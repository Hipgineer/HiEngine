#ifndef __SIMBUFFER_H__
#define __SIMBUFFER_H__

#include "common.h"

uint64_t const maxParticle = 1'000'000;

// enum OutputType {
// 	DENSITY,
	
// }

struct CommonParameters {
	float radius;
	float diameter = 2.0f * radius;
	float H = 1.3f * diameter;
	float dt;
	float relaxationParameter;
	float scorrK;
	float scorrDq;
	int32_t iterationNumber;
	CommonParameters() : 
		radius(0.1f), 
		diameter(0.2f),
		H(0.48f),
		dt(0.1f),
		relaxationParameter(0.0000001f),
		scorrK(0.01f),
		scorrDq(0.1f),
		iterationNumber(3)
		{};
};

struct PhaseParameters {
	float density;
	glm::vec3 color; 
	PhaseParameters() : density(1000.0f), color(glm::vec3(1.0f,0.0f,0.0f))
		{};
};

CLASS_PTR(SimBuffer);
class SimBuffer
{
public :
	static SimBufferPtr Create();
	int32_t GetNumParticles() { return m_positions.size(); }

private :
    SimBuffer() {};
	bool Init();

public :
	std::vector<glm::vec3>  m_positions;
	std::vector<glm::vec3>  m_velocities;
	std::vector<int32_t>    m_phases;
	std::vector<float>      m_colorValues;

	CommonParameters m_commonParam;
	std::vector<PhaseParameters> m_phaseParam;


private :
};

#endif // __SIMBUFFER_H__