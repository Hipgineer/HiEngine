#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "framebuffer.h"
#include "simbuffer.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "HiPhysics/hiphysics.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    // void Update(std::vector<glm::vec3>& positions); // g_solver
    bool MapSimBuffer(SimBufferPtr simBuffer);
    void Render();

    void ProcessInput(GLFWwindow *window);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
    void MouseWheel(double xoffset, double yoffset);
    void Reshape(int width, int height);
    void PressKey(int key, int scancode, int action, int mods);

    // animation
    int32_t m_selectedScene {0};

private:
    void RenderFluidDepth();
    void RenderFluidThickness();
    void RenderFluid();
    Context() {};
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;
    ProgramUPtr m_simpleLightingProgram;
    ProgramUPtr m_pointProgram;
    ProgramUPtr m_textureProgram;
    ProgramUPtr m_postProgram; 

    ProgramUPtr m_fluidDepthProgram;
    ProgramUPtr m_fluidThicknessProgram;
    ProgramUPtr m_fluidRenderProgram;

    MeshUPtr m_box;
    MeshUPtr m_plane;
    std::vector<MeshUPtr> m_boxes;
    ModelUPtr m_model;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;
    

    // clear color
    glm::vec4 m_clearColor {glm::vec4(0.1f,0.2f,0.3f,0.0f)};

    // light parameters
    struct Light {
        glm::vec3 position { glm::vec3(0.1f,0.1f,0.1f) };
        glm::vec3 direction{glm::vec3(-1.0f, -1.0f, -1.0f)};
        glm::vec2 cutoff {glm::vec2(180.0f, 0.0f)};
        float distance {300.0f};
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;
    bool m_flashLightMode { true };

    // material parameters
    struct Material {
        TextureUPtr diffuse;
        TextureUPtr specular;
        float shininess { 32.0f };
    };
    Material m_material;
    
    struct MaterialBasic {
        glm::vec3 diffuse { glm::vec3(0.4f, 0.8f, 0.8f) };
        glm::vec3 specular { glm::vec3(0.0f, 0.0f, 0.0f) };
        float shininess { 32.0f };
    };
    MaterialBasic m_materialBasic;

    // camera parameters
    float m_cameraSpeedRatio {1.0f};
    bool m_cameraControl {false};
    glm::vec2 m_prevMousePos {glm::vec2(0.0f)};
    float m_cameraPitch {0.0f};
    float m_cameraYaw {0.0f};
    glm::vec3 m_cameraPos { glm::vec3(0.0f,0.0f,3.0f)};
    glm::vec3 m_cameraFront { glm::vec3(0.0f,0.0f,-1.0f)};
    glm::vec3 m_cameraUp { glm::vec3(0.0f,1.0f,0.0f)};

    // framebuffer
    FramebufferUPtr m_framebuffer;

    // Particle Size
    float m_particleSizeRatio {1.0f};

    // Legend
    bool m_autoLegend {true};
    float m_minLegend {0.0f};
    float m_maxLegend {1.0f};
    
    // std::shared_ptr<std::vector<glm::vec3>> m_positions; 
    std::vector<glm::vec3> * m_positions;
    std::vector<float> * m_colors;
    CommonParameters * m_commonParam;

    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};
};

#endif //__CONTEXT_H__