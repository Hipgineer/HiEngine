# HiEngine
Hipgineer's Physics-based Simulation Engine.

## Key Features
- PBF fluid simulator
- PBD cloth simulator
- CUDA Acceleration
- Screen Space Fluid Rendering (Green, 2010)

## Dependencies
- spdlog, glfw, glad, stb, glm, imgui, assimp

## How to generate a scene
1. describe a scene by inheriting the scene class.
    ```
    // scenes/yourScene.h
    // 
    class YourScene : public Scene
    {
    public :
        YourScene(const char* name) : Scene(name) {}

        virtual void Init()
        {
            // describing your scene.
        }
    };
    ```
2. include your scene in scene.h
    
    ```#include "yourScene.h"```
    
3. load the scene in main.cpp

    ```g_scenes.push_back(new YourScene("My Test Scene")); ```
    
4. build and run this code. check your scene in the ui scene list

![Alt Text](https://github.com/Hipgineer/HiEngine/blob/main/doc/img/myTestScene.png)


## Examples

*Dam Break* | *Sphere Drop* | *Cloth*
--- | --- | ---
![Alt Text](https://github.com/Hipgineer/HiEngine/blob/main/doc/img/ex_dambreak.gif) | ![Alt Text](https://github.com/Hipgineer/HiEngine/blob/main/doc/img/ex_spheredrop.gif) | ![Alt Text](https://github.com/Hipgineer/HiEngine/blob/main/doc/img/ex_cloth.gif)
