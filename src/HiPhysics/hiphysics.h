#ifndef __HIPHYSICS_H__
#define __HIPHYSICS_H__

#include "../core/point3.h"
#include "../core/vec3.h"
#include "../src/common.h"
#include "../src/simbuffer.h"


struct DeviceData{

    int32_t* gridIndices;      // Particle Grid Index
    int32_t* numPartInGrids;   // Particle Number of PArticles in each Grid
    int32_t* nearGridID;       // Near Grid IDs of each Grids

    // Interchangable Data
    float* colorValues;
    glm::vec3* positions;      // Particle Positions
    glm::vec3* velocities;     // Particle Velocities
    int32_t* phases;           // Particle Phase number
    
    // Only for Device
    float* constraints;        // Particle Constraints
    float* lambdas;            // Particle Lambdas
    glm::vec3* deltaPos;       // Particle Positions Displace 
    glm::vec3* correctedPos;   // Particle Corrected Positions
    
    // Parameters
	CommonParameters* commonParam;
    PhaseParameters* phaseParam;
    //
    DeviceData() : 
        gridIndices(nullptr),
        numPartInGrids(nullptr),
        nearGridID(nullptr),

        colorValues(nullptr), 
        positions(nullptr),
        velocities(nullptr),
        phases(nullptr),

        constraints(nullptr),
        lambdas(nullptr),
        correctedPos(nullptr),
        
        commonParam(nullptr),
        phaseParam(nullptr)
        {};
};

CLASS_PTR(HiPhysics);
class HiPhysics {
public:
    static HiPhysicsUPtr Create();

    // Memory Functions

    bool ClearMemory();
    
    bool SetMemory(SimBufferPtr simBuffer);
    
    bool GetMemory(SimBufferPtr simBuffer);
    
    bool MemsetFromHost(SimBufferPtr simBuffer);
    
    // Physics Functions

	void UpdateSolver(SimBufferPtr simBuffer);
    
    bool PredictPosition(SimBufferPtr simBuffer);

    bool ComputeGridIndices(SimBufferPtr simBuffer);
    
    bool SortVariablesByIndices(SimBufferPtr simBuffer);
    
    bool ComputeConstraint(SimBufferPtr simBuffer);
    
    bool UpdateVelPos(SimBufferPtr simBuffer);

    bool GetRenderingVariable(SimBufferPtr simBuffer);

    uint32_t GetActiveCount() const { return m_numParticles; }

private:

    HiPhysics() {};

    bool Init();
    
    uint32_t m_numParticles { 0 };

    uint32_t m_numFluidParticles { 0 };

    DeviceData dm_Data {};
};

#endif // __HIPHYSICS_H__