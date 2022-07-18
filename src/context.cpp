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

    // m_box = Mesh::CreateSphere(10, 10, 0.05f);
    m_box = Mesh::CreateBox(glm::vec3(-0.01f,-0.01f,-0.01f),glm::vec3(0.01f,0.01f,0.01f));

    // m_points = Points::Create();

    m_model = Model::Load("./models/backpack/backpack.obj");
    if (!m_model)
        return false;

    // Loading Programs
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

    // Initializing openGL Scene
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

bool Context::MapSimBuffer(SimBufferPtr simBuffer)
{
    // Copy Address
    m_positions = &simBuffer->m_positions; 
    m_colors = &simBuffer->m_colorValues;
    m_commonParam = &simBuffer->m_commonParam;
    return true;
    // uint64_t count = (int)(positions->size());
    // if (count)
    // {
    //     m_positions.resize(count);
    //     std::copy(positions->begin(), positions->end(), m_positions.begin());
    //     return true;
    // }
    // else
    //     return false;
}

void Context::Render()
{
    // imgui - setting GUI
    if (ImGui::Begin("ui window"))
    {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        ImGui::Separator();
        // ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        // ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        // ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::DragFloat("camera speed", &m_cameraSpeedRatio, 0.001f, 0.001f, 100.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera"))
        {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
            m_cameraSpeedRatio = 1.0f;
        }
        if (ImGui::Button("Reload Scene"))
        {
            m_reloadScene = true;
        }
        
        ImGui::Separator();
        ImGui::DragFloat("particle size", &m_particleSizeRatio, 0.01f, 0.01f, 2.0f);
        ImGui::Separator();
        
        ImGui::Separator();
        ImGui::Checkbox("legend auto", &m_autoLegend);
        ImGui::InputFloat("legend min", &m_minLegend,0.1f, 0.2f, "%.10f");
        ImGui::InputFloat("legend max", &m_maxLegend,0.1f, 0.2f, "%.10f");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Numerical Parameters", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderInt("Iterations", &m_commonParam->iterationNumber,1,30);
            ImGui::InputFloat("particle radius", &m_commonParam->radius,
                                                    0.1f*m_commonParam->radius, 
                                                    0.2f*m_commonParam->radius, "%.5f");
            ImGui::InputFloat("compute time step", &m_commonParam->dt,
                                                    0.1f*m_commonParam->dt, 
                                                    0.2f*m_commonParam->dt, "%.5f");
            ImGui::InputFloat("relaxationParameter", &m_commonParam->relaxationParameter,
                                                    0.1f*m_commonParam->relaxationParameter, 
                                                    0.2f*m_commonParam->relaxationParameter, "%.5f");
            ImGui::InputFloat("scorrK", &m_commonParam->scorrK,
                                                    0.1f*m_commonParam->scorrK, 
                                                    0.2f*m_commonParam->scorrK, "%.5f");
            ImGui::InputFloat("scorrDq", &m_commonParam->scorrDq,
                                                    0.1f*m_commonParam->scorrDq, 
                                                    0.2f*m_commonParam->scorrDq, "%.5f");
        }
        

        // if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        // {
        //     ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
        //     ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
        //     ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 180.0f);
        //     ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
        //     ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
        //     ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
        //     ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
        // }

        // if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen))
        // {
        //     ImGui::ColorEdit3("m.diffuse", glm::value_ptr(m_materialBasic.diffuse));
        //     ImGui::ColorEdit3("m.specular", glm::value_ptr(m_materialBasic.specular));
        //     ImGui::DragFloat("m.shininess", &m_materialBasic.shininess, 1.0f, 1.0f, 256.0f);
        // }

        // ImGui::Checkbox("flash light", &m_flashLightMode);
    }
    ImGui::End();

    // opengl - intialize frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);  

    // opengl - render elements
    float fov = 45.0f;
    float aspect = (float)m_width / (float)m_height;
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    auto proj = glm::perspective(glm::radians(fov), aspect, 0.01f, 10.0f);

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
        auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position);
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", proj * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }
    // Legend
    if (m_autoLegend)
    {
        m_minLegend = static_cast<float>(*std::min_element(m_colors->begin(), m_colors->end()));
        m_maxLegend = static_cast<float>(*std::max_element(m_colors->begin(), m_colors->end()));
    }

    // TODO : Refactoring Like m_box
    // linking Point shader buffers

    auto pointVertexLayout = VertexLayout::Create();
    auto pointVertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        m_positions->data(), sizeof(glm::vec3), m_positions->size());
    pointVertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
    auto pointVertexColorBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        m_colors->data(), sizeof(float), m_colors->size());
    pointVertexLayout->SetAttrib(1, 1, GL_FLOAT, false, sizeof(float), 0);

    auto pointTransform =  - glm::rotate(glm::mat4(1.0f), glm::radians(-m_cameraPitch/2.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                            glm::rotate(glm::mat4(1.0f), glm::radians(-m_cameraYaw/2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_pointProgram->Use();
    m_pointProgram->SetUniform("transform", proj*view);
    m_pointProgram->SetUniform("viewTransform", view);
    m_pointProgram->SetUniform("pointTransform", pointTransform);
    m_pointProgram->SetUniform("pointRadius", m_particleSizeRatio*m_commonParam->radius);
    m_pointProgram->SetUniform("pointScale", (float)m_width/aspect * (1.0f / glm::tan(glm::radians(fov*0.5f)))); 
    m_pointProgram->SetUniform("colorMin", m_minLegend);
    m_pointProgram->SetUniform("colorMax", m_maxLegend);
    m_pointProgram->SetUniform("cameraPos", m_cameraPos);
    m_pointProgram->SetUniform("light.position", lightPos);
    m_pointProgram->SetUniform("light.direction", lightDir);
    m_pointProgram->SetUniform("light.cutoff", glm::vec2(   cosf(glm::radians(m_light.cutoff[0])),
                                                            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_pointProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_pointProgram->SetUniform("light.ambient", m_light.ambient);
    m_pointProgram->SetUniform("light.diffuse", m_light.diffuse);
    m_pointProgram->SetUniform("light.specular", m_light.specular);

    m_pointProgram->SetUniform("material.diffuse", m_materialBasic.diffuse);
    m_pointProgram->SetUniform("material.specular", m_materialBasic.specular);
    m_pointProgram->SetUniform("material.shininess", m_materialBasic.shininess);

    pointVertexLayout->Bind();
    glDrawArrays(GL_POINTS, 0, m_positions->size());
}

void Context::ProcessInput(GLFWwindow *window)
{
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraSpeedRatio * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraSpeedRatio * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraSpeedRatio * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraSpeedRatio * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraSpeedRatio * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraSpeedRatio * cameraUp;
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
    const float cameraSpeed = 1.0f;
    m_cameraPos += glm::vec3(cameraSpeed*m_cameraSpeedRatio * yoffset) * m_cameraFront;
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::PressKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
        m_cameraSpeedRatio *= 0.4f;
    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
        m_cameraSpeedRatio *= 2.5f;
}