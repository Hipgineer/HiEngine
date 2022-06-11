#ifndef __SIMBUFFER_H__
#define __SIMBUFFER_H__

#include "common.h"

uint64_t const maxParticle = 1'000'000;

CLASS_PTR(SimBuffer);
class SimBuffer
{
public :
	static SimBufferPtr Create();

private :
    SimBuffer() {};
	bool Init();

public :
	std::vector<glm::vec3>  m_positions;
	std::vector<glm::vec3>  m_velocities;
	std::vector<int8_t>     m_phases;
	std::vector<float>      m_densities;

	float m_radius = 0.01;


private :
};

#endif // __SIMBUFFER_H__