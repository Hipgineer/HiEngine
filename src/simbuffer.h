#ifndef __SIMBUFFER_H__
#define __SIMBUFFER_H__

#include "common.h"

uint64_t const maxParticle = 1'000'000;
struct CommonParameters {
	float radius;
	float dt;
	CommonParameters() : radius(0.1f), dt(0.1f) {};
};

CLASS_PTR(SimBuffer);
class SimBuffer
{
public :
	static SimBufferPtr Create();
	int64_t GetNumParticles() { return m_positions.size(); }

private :
    SimBuffer() {};
	bool Init();

public :
	std::vector<glm::vec3>  m_positions;
	std::vector<glm::vec3>  m_velocities;
	std::vector<int32_t>    m_phases;
	std::vector<float>      m_densities;

	CommonParameters m_commonParam;


private :
};

#endif // __SIMBUFFER_H__