#ifndef __HIPHYSICS_H__
#define __HIPHYSICS_H__

#include "../core/point3.h"
#include "../core/vec3.h"
#include "../src/common.h"
#include "../src/simbuffer.h"
struct DeviceSimParams{

	CommonParameters* commonParam;
    PhaseParameters* phaseParam;

    DeviceSimParams() :        
        commonParam(nullptr),
        phaseParam(nullptr)
        {};
};

struct DeviceParticleData{
    // To search near particles.
    int32_t* gridIndices;      // Particle Grid Index
    int32_t* numPartInGrids;   // Particle Number of PArticles in each Grid
    int32_t* nearGridID;       // Near Grid IDs of each Grids
    
    // Interchangable Data with Host
    float* colorValues;
    glm::vec3* positions;      // Particle Positions
    glm::vec3* velocities;     // Particle Velocities
    int32_t* phases;           // Particle Phase number
    
    // To compute next position
    glm::vec3* deltaPos;       // Particle Positions Displace 
    glm::vec3* correctedPos;   // Particle Corrected Positions

    DeviceParticleData() : 
        gridIndices(nullptr),
        numPartInGrids(nullptr),
        nearGridID(nullptr),

        colorValues(nullptr), 
        positions(nullptr),
        velocities(nullptr),
        phases(nullptr),

        deltaPos(nullptr),
        correctedPos(nullptr)
        {};  
};

struct DeviceDataFluid : DeviceParticleData{

    // Only for Device
    float* constraints;        // Particle Constraints
    float* lambdas;            // Particle Lambdas
    
    // Parameters : TODO : move to simParameters
	CommonParameters* commonParam;
    PhaseParameters* phaseParam;
    //
    DeviceDataFluid() :
        constraints(nullptr),
        lambdas(nullptr),
        
        commonParam(nullptr),
        phaseParam(nullptr)
        {};
};


struct DeviceDataCloth : DeviceParticleData{
    int32_t* stretchID; 
    int32_t* bendID; 
    int32_t* shearID; 
    int32_t* triangles;

    DeviceDataCloth() :
        stretchID(nullptr),
        bendID(nullptr),
        shearID(nullptr),
        triangles(nullptr)
        {};
};

CLASS_PTR(HiPhysics);
class HiPhysics {
public:
    static HiPhysicsUPtr Create();

    // Memory Functions

    bool ClearMemory();
    
    bool SetMemory(SimBufferPtr simBuffer);

    bool SetMemoryCloth(SimBufferPtr simBuffer);
    
    bool GetMemory(SimBufferPtr simBuffer);
    
    bool MemsetFromHost(SimBufferPtr simBuffer);
    
    bool MemsetFromHostCloth(SimBufferPtr simBuffer);
    
    // Physics Functions

	void UpdateSolver(SimBufferPtr simBuffer);
    
    bool PredictPosition(SimBufferPtr simBuffer);

    bool ComputeGridIndices(SimBufferPtr simBuffer);
    
    bool SortVariablesByIndices(SimBufferPtr simBuffer);
    
    bool ComputeConstraint(SimBufferPtr simBuffer);
    
    bool UpdateVelPos(SimBufferPtr simBuffer);

    bool GetRenderingVariable(SimBufferPtr simBuffer);

    // Cloth Functions

	void UpdateSolverCloth(SimBufferPtr simBuffer);
    
    bool PredictPositionCloth(SimBufferPtr simBuffer);

    bool ComputeConstraintCloth(SimBufferPtr simBuffer);
    
    bool UpdateVelPosCloth(SimBufferPtr simBuffer);
    
    bool GetRenderingVariableCloth(SimBufferPtr simBuffer);

    bool GetMemoryCloth(SimBufferPtr simBuffer);


    uint32_t GetActiveCount() const { return m_numParticles; }

private:

    HiPhysics() {};

    bool Init();
    
    uint32_t m_numParticles { 0 };

    uint32_t m_numFluidParticles { 0 };

    DeviceSimParams dm_SimParameters {};

    DeviceDataFluid dm_DataFluid {};

    DeviceDataCloth dm_DataCloth {};
    
};

#endif // __HIPHYSICS_H__