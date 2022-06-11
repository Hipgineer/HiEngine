#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init()
{

    m_box = Mesh::CreateSphere(10, 10, 0.05f);
    m_model = Model::Load("./models/backpack/backpack.obj");
    if (!m_model)
        return false;

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;

    m_simpleLightingProgram = Program::Create("./shader/simpleLighting.vs", "./shader/simpleLighting.fs");
    if (!m_simpleLightingProgram)
        return false;

    m_program = Program::Create("./shader/objLighting.vs", "./shader/objLighting.fs");
    if (!m_program)
        return false;

    m_pointProgram = Program::Create("./shader/simplePoint.vs","./shader/simplePoint.fs");
    if (!m_pointProgram)
        return false;

    // Initializing
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // default background color

    // Loading Texture Images
    auto image = Image::Load("./image/container.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels",
                image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./image/awesomeface.png");
    if (!image2)
        return false;
    SPDLOG_INFO("image2: {}x{}, {} channels",
                image2->GetWidth(), image2->GetHeight(), image2->GetChannelCount());
    m_texture2 = Texture::CreateFromImage(image2.get());

    // m_material.diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    // m_material.specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());

    m_material.diffuse = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

    m_material.specular = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    return true;
}

bool Context::UpdateScene(std::vector<glm::vec3> *positions)
{
    uint64_t count = (int)(positions->size());
    if (count)
    {
        m_positions.resize(count);
        std::copy(positions->begin(), positions->end(), m_positions.begin());
        return true;
    }
    else
        return false;
}

void Context::Render()
{
    // imgui - setting GUI
    if (ImGui::Begin("ui window"))
    {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera"))
        {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }

        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
        }

        if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("m.shininess", &m_material.shininess, 1.0f, 1.0f, 256.0f);
        }

        ImGui::Checkbox("animation", &m_pause);
        ImGui::Checkbox("flash light", &m_flashLightMode);
    }
    ImGui::End();

    // opengl - intialize frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);  

    // opengl - render elements
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    auto model = glm::rotate(glm::mat4(1.0f), glm::radians((float)m_timestep), glm::vec3(1.0f, 0.5f, 0.0f));
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    auto proj = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 10.0f);

    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if (m_flashLightMode)
    {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }
    else
    {
        // after computing projection and view matrix
        auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", proj * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }

    m_simpleLightingProgram->Use();
    m_simpleLightingProgram->SetUniform("viewPos", m_cameraPos);
    m_simpleLightingProgram->SetUniform("light.position", lightPos);
    m_simpleLightingProgram->SetUniform("light.direction", lightDir);
    m_simpleLightingProgram->SetUniform("light.cutoff", glm::vec2(
                                                            cosf(glm::radians(m_light.cutoff[0])),
                                                            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_simpleLightingProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_simpleLightingProgram->SetUniform("light.ambient", m_light.ambient);
    m_simpleLightingProgram->SetUniform("light.diffuse", m_light.diffuse);
    m_simpleLightingProgram->SetUniform("light.specular", m_light.specular);

    m_simpleLightingProgram->SetUniform("material.diffuse", m_materialBasic.diffuse);
    m_simpleLightingProgram->SetUniform("material.specular", m_materialBasic.specular);
    m_simpleLightingProgram->SetUniform("material.shininess", m_materialBasic.shininess);

    // m_program->SetUniform("material.diffuse", 0);
    // m_program->SetUniform("material.specular", 1);
    // m_program->SetUniform("material.shininess", m_material.shininess);
    // glActiveTexture(GL_TEXTURE0);
    // m_material.diffuse->Bind();
    // glActiveTexture(GL_TEXTURE1);
    // m_material.specular->Bind();

    // auto modelTransform = glm::translate(glm::mat4(1.0), glm::vec3((float)m_timestep*0.01f, 0.0f,0.0f));
    // auto models = glm::translate(glm::mat4(1.0), glm::vec3((float)m_timestep*0.01f, 0.0f,0.0f));

    // TODO :
    // - There must be better way to render 
    //   these many particles simultenuously.
    //   not through a loop!
    // std::vector<glm::vec3>::iterator ptr;
    // for (ptr = m_positions.begin(); ptr != m_positions.end(); ++ptr)
    // {
    //     auto modelTransform = glm::translate(glm::mat4(1.0), *ptr);
    //     auto transform = proj * view * modelTransform;
    //     m_simpleLightingProgram->SetUniform("transform", transform);
    //     m_simpleLightingProgram->SetUniform("modelTransform", modelTransform);
    //     m_box->Draw(m_simpleLightingProgram.get());
    // }

        auto pointVertexLayout = VertexLayout::Create();
        auto pointVertexBuffer = Buffer::CreateWithData(
            GL_ARRAY_BUFFER, GL_STATIC_DRAW,
            m_positions.data(), sizeof(glm::vec3), m_positions.size());
        pointVertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);


        m_pointProgram->Use();
        m_pointProgram->SetUniform("transform", proj*view);
        m_pointProgram->SetUniform("modelTransform", view);
        m_pointProgram->SetUniform("pointRadius", 0.1f);
        m_pointProgram->SetUniform("pointScale", static_cast<float>(m_width)); 
        // screenWidth/screenAspect * (1.0f / (tanf(fov*0.5f))))
        // 2048       / 1.0f        * (1.0f / (tanf(30 degree)) 
        pointVertexLayout->Bind();
        glDrawArrays(GL_POINTS, 0, m_positions.size());
        // glDrawArrays(GL_TRIANGLES, 0, m_positions.size());

    if (!m_pause || m_step)
    {
        m_timestep++;
        m_step = false;
    }
}

void Context::ProcessInput(GLFWwindow *window)
{
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f)
        m_cameraYaw -= 360.0f;
    if (m_cameraPitch > 89.0f)
        m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f)
        m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
        {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}

void Context::MouseWheel(double xoffset, double yoffset)
{
    const float cameraSpeed = 0.5f;
    m_cameraPos += glm::vec3(cameraSpeed * yoffset) * m_cameraFront;
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::PressKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        m_pause = !m_pause;
    if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT))
        m_step = true;
}