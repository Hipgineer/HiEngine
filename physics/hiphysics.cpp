#include "hiphysics.h"

HiPhysicsUPtr HiPhysics::Create() {
    auto solver = HiPhysicsUPtr(new HiPhysics());
    if(!solver->Init())
        return nullptr;
    return std::move(solver);
}


void HiPhysics::SetParticles() {

}

void HiPhysics::SetVelocities() {

}

void HiPhysics::SetPhases() {

}

void HiPhysics::SetActive() {

}

void HiPhysics::SetActiveCount() {

}

void HiPhysics::SetParams() {

}

void HiPhysics::UpdateSolver() {

}

bool HiPhysics::Init () {
    
    return true;
}