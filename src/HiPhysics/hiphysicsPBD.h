#ifndef __HIPHYSICSPBD_H__
#define __HIPHYSICSPBD_H__

#include "hiphysics.h"

class HiPhysicsPBD : public HiPhysics
{
public:
	virtual void UpdateSolver();

private:
	__host__ __device__ void SearchNearestNeighbor();
};
#endif // __HIPHYSICSPBD_H__