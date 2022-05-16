#ifndef __HIPHYSICS_H__
#define __HIPHYSICS_H__

#include "../src/common.h"
#include "../core/point3.h"
#include "../core/vec3.h"

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


/**
 * Simulation parameters for a solver
 */

typedef int32_t                 hiInt;
typedef float                   hiScalar;
typedef HiXVector3<hiScalar>    hiVec3;
typedef Point3                  hiPnt3;

struct HiPhysicsParam
{
	int numIterations;					//!< Number of solver iterations to perform per-substep

	hiScalar gravity[3];					//!< Constant acceleration applied to all particles
	hiScalar radius;						//!< The maximum interaction radius for particles
	hiScalar solidRestDistance;			//!< The distance non-fluid particles attempt to maintain from each other, must be in the range (0, radius]
	hiScalar fluidRestDistance;			//!< The distance fluid particles are spaced at the rest density, must be in the range (0, radius], for fluids this should generally be 50-70% of mRadius, for rigids this can simply be the same as the particle radius

	// common params
	hiScalar dynamicFriction;				//!< Coefficient of friction used when colliding against shapes
	hiScalar staticFriction;				//!< Coefficient of static friction used when colliding against shapes
	hiScalar particleFriction;				//!< Coefficient of friction used when colliding particles
	hiScalar restitution;					//!< Coefficient of restitution used when colliding against shapes, particle collisions are always inelastic
	hiScalar adhesion;						//!< Controls how strongly particles stick to surfaces they hit, default 0.0, range [0.0, +inf]
	hiScalar sleepThreshold;				//!< Particles with a velocity magnitude < this threshold will be considered fixed
	
	hiScalar maxSpeed;						//!< The magnitude of particle velocity will be clamped to this value at the end of each step
	hiScalar maxAcceleration;				//!< The magnitude of particle acceleration will be clamped to this value at the end of each step (limits max velocity change per-second), useful to avoid popping due to large interpenetrations
	
	hiScalar shockPropagation;				//!< Artificially decrease the mass of particles based on height from a fixed reference point, this makes stacks and piles converge faster
	hiScalar dissipation;					//!< Damps particle velocity based on how many particle contacts it has
	hiScalar damping;						//!< Viscous drag force, applies a force proportional, and opposite to the particle velocity

	// cloth params
	hiScalar wind[3];						//!< Constant acceleration applied to particles that belong to dynamic triangles, drag needs to be > 0 for wind to affect triangles
	hiScalar drag;							//!< Drag force applied to particles belonging to dynamic triangles, proportional to velocity^2*area in the negative velocity direction
	hiScalar lift;							//!< Lift force applied to particles belonging to dynamic triangles, proportional to velocity^2*area in the direction perpendicular to velocity and (if possible), parallel to the plane normal

	// fluid params
	hiScalar cohesion;						//!< Control how strongly particles hold each other together, default: 0.025, range [0.0, +inf]
	hiScalar surfaceTension;				//!< Controls how strongly particles attempt to minimize surface area, default: 0.0, range: [0.0, +inf]    
	hiScalar viscosity;					//!< Smoothes particle velocities using XSPH viscosity
	hiScalar vorticityConfinement;			//!< Increases vorticity by applying rotational forces to particles
	hiScalar anisotropyScale;				//!< Control how much anisotropy is present in resulting ellipsoids for rendering, if zero then anisotropy will not be calculated, see NvFlexGetAnisotropy()
	hiScalar anisotropyMin;				//!< Clamp the anisotropy scale to this fraction of the radius
	hiScalar anisotropyMax;				//!< Clamp the anisotropy scale to this fraction of the radius
	hiScalar smoothing;					//!< Control the strength of Laplacian smoothing in particles for rendering, if zero then smoothed positions will not be calculated, see NvFlexGetSmoothParticles()
	hiScalar solidPressure;				//!< Add pressure from solid surfaces to particles
	hiScalar freeSurfaceDrag;				//!< Drag force applied to boundary fluid particles
	hiScalar buoyancy;						//!< Gravity is scaled by this value for fluid particles

	// diffuse params
	hiScalar diffuseThreshold;				//!< Particles with kinetic energy + divergence above this threshold will spawn new diffuse particles
	hiScalar diffuseBuoyancy;				//!< Scales force opposing gravity that diffuse particles receive
	hiScalar diffuseDrag;					//!< Scales force diffuse particles receive in direction of neighbor fluid particles
	hiInt diffuseBallistic;				//!< The number of neighbors below which a diffuse particle is considered ballistic
	hiScalar diffuseLifetime;				//!< Time in seconds that a diffuse particle will live for after being spawned, particles will be spawned with a random lifetime in the range [0, diffuseLifetime]

	// collision params
	hiScalar collisionDistance;			//!< Distance particles maintain against shapes, note that for robust collision against triangle meshes this distance should be greater than zero
	hiScalar particleCollisionMargin;		//!< Increases the radius used during neighbor finding, this is useful if particles are expected to move significantly during a single step to ensure contacts aren't missed on subsequent iterations
	hiScalar shapeCollisionMargin;			//!< Increases the radius used during contact finding against kinematic shapes

	hiScalar planes[8][4];					//!< Collision planes in the form ax + by + cz + d = 0
	hiInt numPlanes;						//!< Num collision planes

	//NvFlexRelaxationMode relaxationMode;//!< How the relaxation is applied inside the solver
	//float relaxationFactor;				//!< Control the convergence rate of the parallel solver, default: 1, values greater than 1 may lead to instability
};

struct HiParticle {
    int8_t  property;
    Point3  pos;
    Vec3    vel;
    float   constraint;
    float   lambda;         
};

struct HiParticleProperty {
    float radius;
};
struct HiFluidProperty : public HiParticleProperty {
    float viscosity;
};

struct HiSolidProperty : public HiParticleProperty {
    float kspring;
};

CLASS_PTR(HiPhysics);
class HiPhysics {
public:
    static HiPhysicsUPtr Create();

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
    HiPhysics() {};
    bool Init();
    
    // m_solver { 0 } ;
    uint32_t const m_numParticles { 0 } ;
    uint32_t const m_numFluidParticles { 0 } ;

};



#endif // __HIPHYSICS_H__