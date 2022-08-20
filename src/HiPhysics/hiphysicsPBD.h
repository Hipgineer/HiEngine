#ifndef __HIPHYSICSPBD_H__
#define __HIPHYSICSPBD_H__

#include "hiphysics.h"
#include <thrust/device_vector.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/gather.h>

__global__ void keGetRenderValues(
    DeviceData dData,
    int64_t nParticles);

/// Searching K-Nearest Particles
// -> Indexing Grid ID where the paricle is located.
__global__ void keComputeGridID(
    DeviceData dData,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void keCountParticlesInGrids(
    DeviceData dData,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void keComputeConstraint(
    DeviceData dData,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void keComputePositionCorrection(
    DeviceData dData,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void kePredictPosition(
    DeviceData dData,
    int64_t nParticles);

__global__ void keUpdateCorretedPosition(
    DeviceData dData,
    int64_t nParticles);

/// Update Particles Positions
__global__ void keUpdateVelPos(
    DeviceData dData,
    int64_t nParticles);

#endif // __HIPHYSICSPBD_H__