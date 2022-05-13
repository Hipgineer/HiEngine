#include "hiphysics.h"

HiPhysicsSolverUPtr HiPhysicsSolver::Create() {
    auto solver = HiPhysicsSolverUPtr(new HiPhysicsSolver());
    if(!solver->Init())
        return nullptr;
    return std::move(solver);
}


void HiPhysicsSolver::SetParticles() {

}

void HiPhysicsSolver::SetVelocities() {

}

void HiPhysicsSolver::SetPhases() {

}

void HiPhysicsSolver::SetActive() {

}

void HiPhysicsSolver::SetActiveCount() {

}

void HiPhysicsSolver::SetParams() {

}

void HiPhysicsSolver::UpdateSolver() {

}

bool HiPhysicsSolver::Init () {
    return true;
}