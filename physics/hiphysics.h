#ifndef __HIPHYSICS_H__
#define __HIPHYSICS_H__

#include "../src/common.h"

/// Wrapping it as API/dll
/*
#if _WIN32
#define HI_PHYSICS_API __declspec(dllexport)
#else
#define HI_PHYSICS_API
#endif

extern "C" {

struct HiPhysicsSolver {
    int numParticles;
};


HI_PHYSICS_API int HiPhysicsGetActiveCount(HiPhysicsSolver* solver);

}; // extern "c"
*/

CLASS_PTR(HiPhysicsSolver);
class HiPhysicsSolver {
public:
    static HiPhysicsSolverUPtr Create();

    // Set Functions
    // -- from buffer to solver
    void SetParticles();
    void SetVelocities();
    void SetPhases();
    void SetActive();
    void SetActiveCount();

    // Update Frame
    void SetParams();
    void UpdateSolver();

    // Get Functions
    uint32_t GetActiveCount() const { return m_numParticles; }

private:
    HiPhysicsSolver() {};
    bool Init();
    
    uint32_t const m_numParticles { 0 } ;
    uint32_t const m_numFluidParticles { 0 } ;
};



#endif // __HIPHYSICS_H__