#ifndef __SCENES_H__
#define __SCENES_H__

#include "common.h"

CLASS_PTR(Scene);
class Scene
{
public :
    Scene(const char* name) : mName(name) {}

	virtual void Init() = 0;
    
	const char* mName;
};

#include "sphereDrop.h"
#include "sphereCollision.h"
#include "damBreak.h"

#endif // __SCENES_H__
/*
Scene Description Language
Layer를 겹치는 것 같다.

USD가 html이라면 
Omniverse는 브라우져이다. 

pixar 가 최고다
Nvidia와 Apple이 관심을 가지면서 계속 개선해 나가고 있다.

- omniverse에서 만든 카트를 만듬
- Prims API
- Units (inches centimeter) - User define
(
    defaultPrim     = "World"
    startTimeCode   = 0
    endTimeCode     = 100
    metersPerUnit   = 1
    timeCodesPerSecond = 60
    kilogramsPerUnit = 1.0
    upAxis = "Y"
)
def Xform "World"
{
    // def PhysicsScene "physicsScene"
    // {
    //     vector3f physics:gravityDirection = (0, -1, 0)
    //     float physics:gravityMagnitude = 9.8
    // }
    def PhysicsScene "physicsScene" ( prepend apiSchemas = ["PhysxSceneAPI"])
    {
        vector3f physics:gravityDirection = (0, -1, 0)
        float physics:gravityMagnitude = 9.8

        uniform token physxScene:broadphaseType = "MBP"
        uniform token physxScene:collisionSystem = "PCM"
        bool physxScene:enableEngancedDeterminism = 0
        bool physxScene:enableGPUDynamics = 0
        bool physxScene:enableStabilization = 1

        bool physxScene:enableCCD = 0 // Continuous Collition Detection
        uint physxScene:timeStepsPerSecond = 60
    }

    def Mesh "StaticPlane" (
        prepend apiScemas = ["PhysicsCollisionAPI", "PhysicsMeshCollisionAPI"]
    )
    {
        token visibility = "inherited"
        uniform bool doubleSided = 0

        rel material:binding = </World/Looks/Asphalt> (
            bindMateiralAs ="weakerThanDescendants"
        )

        int[] faceVertexCounts = [4]
        int[] faceVertexIndices= [0, 1, 2, 3]
        normal3f[] normals = [(0,1,0), (0,1,0), (0,1,0), (0,1,0)]
        point3f[] points = [(-400,0,-400), (400,0,-400),(400,0,400),(-400,0,400)]
        color3f[] primvars:displayColor = [(0.217638, 0.217638, 0.217638)]

        float3 xformOp:rotateXYZ (180,0,0)
        float3 xformOp:scale = (1,1,1)
        double3 xformOp:translate = (0,0,0)
        uniform token[] xformOpOrder = ["xformOp:translate", "xformOp:rotateXYZ","xformOp:scale"]

        bool physics:collisionEnabled = 1
        rel physics:simulationOwner = <World/physicsScene>
        uniform token physics:approximation = "none"
    }
}

-- forces 

*/
