#include "simbuffer.h"


SimBufferPtr SimBuffer::Create() {
    auto simbuffer = SimBufferPtr(new SimBuffer());
    if(!simbuffer->Init())
        return nullptr;
    return std::move(simbuffer);
};

bool SimBuffer::Init () {
    m_positions.reserve(maxParticle); 
    m_velocities.reserve(maxParticle);
    m_phases.reserve(maxParticle);
    m_densities.reserve(maxParticle);

    m_positions.resize(0);
    m_velocities.resize(0);
    m_phases.resize(0);
    m_densities.resize(0);
    return true;
}