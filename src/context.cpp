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
    //m_box = Mesh::CreateBox(glm::vec3(-0.01f,-0.01f,-0.01f),glm::vec3(0.01f,0.01f,0.01f));

    // Loading Programs
    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;

    m_simpleLightingProgram = Program::Create("./shader/simpleLighting.vs", "./shader/simpleLighting.fs");
    if (!m_simpleLightingProgram)
        return false;

    m_pointProgram = Program::Create("./shader/simplePoint.vs","./shader/simplePoint.fs");
    if (!m_pointProgram)
        return false;

    m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");
    if (!m_textureProgram)
        return false;

    m_fluidDepthProgram = Program::Create("./shader/fluidDepth.vs", "./shader/fluidDepth.fs");
    if (!m_fluidDepthProgram)
        return false;
        
    m_fluidThicknessProgram = Program::Create("./shader/fluidThickness.vs", "./shader/fluidThickness.fs");
    if (!m_fluidThicknessProgram)
        return false;

    m_fluidRenderProgram = Program::Create("./shader/fluidRender.vs", "./shader/fluidRender.fs");
    if (!m_fluidRenderProgram)
        return false;


    // Initializing openGL Scene
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // default background color

    m_plane = Mesh::CreatePlane();

    return true;
}

bool Context::MapSimBuffer(SimBufferPtr simBuffer)
{
    // Copy Address
    m_positions = &simBuffer->m_positions; 
    m_colors = &simBuffer->m_colorValues;
    m_commonParam = &simBuffer->m_commonParam;
    return true;
}

void Context::RenderFluidDepth()
{

}

void Context::RenderFluidThickness()
{

}

void Context::DrawUI()
{
    // imgui - setting GUI
    if (ImGui::Begin("ui window"))
    {

        if (ImGui::BeginListBox("Scenes",ImVec2(0.0f, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int32_t sceneIdx = 0 ; sceneIdx < m_sceneList.size() ; ++sceneIdx)
            {
                const bool is_selected = (sceneIdx == m_selectedScene);
                if (ImGui::Selectable(m_sceneList[sceneIdx], is_selected))
                {
                    m_selectedScene = sceneIdx;
                    m_reloadScene = true;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
                    
            }
            ImGui::EndListBox();
        }

        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        ImGui::Separator();
        ImGui::DragFloat("camera speed", &m_cameraSpeedRatio, 0.001f, 0.001f, 100.0f);
        ImGui::DragFloat3("light position",  glm::value_ptr(m_light.position), 0.01f);

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
            // m_selectedScene = 0;
            m_reloadScene = true;
        }
        
        ImGui::Separator();
        ImGui::DragFloat("particle size", &m_particleSizeRatio, 0.01f, 0.01f, 2.0f);
        ImGui::Separator();
        
        
        // Legend
        ImGui::Separator();
        ImGui::Checkbox("legend auto", &m_autoLegend);
        if (m_autoLegend)
        {
            m_minLegend = static_cast<float>(*std::min_element(m_colors->begin(), m_colors->end()));
            m_maxLegend = static_cast<float>(*std::max_element(m_colors->begin(), m_colors->end()));
        }
        ImGui::InputFloat("legend min", &m_minLegend,0.1f, 0.2f, "%.10f");
        ImGui::InputFloat("legend max", &m_maxLegend,0.1f, 0.2f, "%.10f");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Numerical Parameters", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderInt("Iterations", &m_commonParam->iterationNumber,1,30);
            if (ImGui::InputFloat("particle radius", &m_commonParam->radius,0.1f*m_commonParam->radius, 0.2f*m_commonParam->radius, "%.5f"))
                m_commonParam->H = m_commonParam->radius * 2.0f * 2.0f * 1.2f;
            ImGui::InputFloat("compute time step", &m_commonParam->dt, 0.1f*m_commonParam->dt, 0.2f*m_commonParam->dt, "%.5f");
            ImGui::InputFloat("relaxationParameter", &m_commonParam->relaxationParameter,0.1f*m_commonParam->relaxationParameter, 0.2f*m_commonParam->relaxationParameter, "%.5f");
            ImGui::InputFloat("scorrK", &m_commonParam->scorrK,0.1f*m_commonParam->scorrK, 0.2f*m_commonParam->scorrK, "%.5f");
            ImGui::InputFloat("scorrDq", &m_commonParam->scorrDq,0.1f*m_commonParam->scorrDq, 0.2f*m_commonParam->scorrDq, "%.5f");
        }
        ImGui::DragFloat3("gravity",  glm::value_ptr(m_commonParam->gravity), 0.01f);

        ImGui::Checkbox("flash light", &m_flashLightMode);
    }
    ImGui::End();
}

void Context::Render()
{
    DrawUI();


    m_framebuffer->Bind();

    // opengl - intialize frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);  


    // opengl - render elements
    
    // Camera Settings
    float fov = 45.0f;
    float aspect = (float)m_width / (float)m_height;
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    auto proj = glm::perspective(glm::radians(fov), aspect, 0.01f, 10.0f);
    
    // Light Settings
    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = (m_commonParam->AnalysisBox.minPoint + m_commonParam->AnalysisBox.maxPoint)*0.5f - m_light.position;

    // Point Vertex Buffer 
    // TODO Modulization
    auto pointVertexLayout = VertexLayout::Create();
    auto pointVertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        m_positions->data(), sizeof(glm::vec3), m_positions->size());
    pointVertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);

    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        // Draw Thickness on FrameBeffer
        m_fluidThicknessProgram->Use(); 
        m_fluidThicknessProgram->SetUniform("transform", proj*view);
        m_fluidThicknessProgram->SetUniform("viewTransform", view);
        m_fluidThicknessProgram->SetUniform("pointRadius", m_particleSizeRatio*m_commonParam->radius);
        m_fluidThicknessProgram->SetUniform("pointScale", (float)m_width/aspect * (1.0f / glm::tan(glm::radians(fov*0.5f))));
        pointVertexLayout->Bind();
        glDrawArrays(GL_POINTS, 0, m_positions->size());


        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE);
    }

    {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        // Draw Depth on FrameBuffer
        m_fluidDepthProgram->Use();
        m_fluidDepthProgram->SetUniform("transform", proj*view);
        m_fluidDepthProgram->SetUniform("projTransform", proj);
        m_fluidDepthProgram->SetUniform("viewTransform", view);
        m_fluidDepthProgram->SetUniform("pointRadius", m_particleSizeRatio*m_commonParam->radius);
        m_fluidDepthProgram->SetUniform("pointScale", (float)m_width/aspect * (1.0f / glm::tan(glm::radians(fov*0.5f))));
        pointVertexLayout->Bind();
        glDrawArrays(GL_POINTS, 0, m_positions->size());

		glDisable(GL_PROGRAM_POINT_SIZE);
    }

    Framebuffer::BindToDefault();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);


    if (m_flashLightMode)
    {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }
    else
    {
        // after computing projection and view matrix
        auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) * glm::scale(glm::mat4(1.0),glm::vec3(0.5));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", proj * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }

    m_fluidRenderProgram->Use();
    m_fluidRenderProgram->SetUniform("transform",
                                 glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
    
    m_fluidRenderProgram->SetUniform("iProjTransform", glm::inverse(proj));
    m_fluidRenderProgram->SetUniform("iViewTransform", glm::inverse(view));
    m_fluidRenderProgram->SetUniform("viewTransform", view);

    m_fluidRenderProgram->SetUniform("light.position", lightPos);
    m_fluidRenderProgram->SetUniform("light.direction", lightDir);
    m_fluidRenderProgram->SetUniform("light.cutoff", glm::vec2(   cosf(glm::radians(m_light.cutoff[0])),
                                                            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_fluidRenderProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_fluidRenderProgram->SetUniform("light.ambient", m_light.ambient);
    m_fluidRenderProgram->SetUniform("light.diffuse", m_light.diffuse);
    m_fluidRenderProgram->SetUniform("light.specular", m_light.specular);

    glActiveTexture(GL_TEXTURE0);
    m_framebuffer->GetColorAttachment()->Bind();
    m_fluidRenderProgram->SetUniform("tex", 0);

    glActiveTexture(GL_TEXTURE1);
    m_framebuffer->GetDepthAttachment()->Bind();
    m_fluidRenderProgram->SetUniform("texDepth", 1);
    
    m_plane->Draw(m_fluidRenderProgram.get());

    // ------ 
    

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

    // m_framebuffer = Framebuffer::Create(
    //     Texture::Create(width, height, GL_RGBA));

    m_framebuffer = Framebuffer::Create(
        Texture::Create(width, height, GL_RGBA), Texture::Create(width, height, GL_DEPTH_COMPONENT));
}

void Context::PressKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
        m_cameraSpeedRatio *= 0.4f;
    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
        m_cameraSpeedRatio *= 2.5f;
}