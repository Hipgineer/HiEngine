#ifndef __HIPHYSICSPBD_H__
#define __HIPHYSICSPBD_H__

#include "hiphysics.h"
#include <thrust/device_vector.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/gather.h>

__global__ void keGetRenderValues(
    DeviceDataFluid dDataFluid,
    int64_t nParticles);

/// Searching K-Nearest Particles
// -> Indexing Grid ID where the paricle is located.
__global__ void keComputeGridID(
    DeviceDataFluid dDataFluid,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void keCountParticlesInGrids(
    DeviceDataFluid dDataFluid,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void keComputeConstraint(
    DeviceDataFluid dDataFluid,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void keComputePositionCorrection(
    DeviceDataFluid dDataFluid,
    glm::vec3 v3MinPosition,
    glm::vec3 v3MaxPosition,
    int64_t nParticles);

__global__ void kePredictPosition(
    DeviceDataFluid dDataFluid,
    int64_t nParticles);

__global__ void keUpdateCorretedPosition(
    DeviceDataFluid dDataFluid,
    int64_t nParticles);

/// Update Particles Positions
__global__ void keUpdateVelPos(
    DeviceDataFluid dDataFluid,
    int64_t nParticles);



__global__ void kePredictPositionCloth(
    DeviceDataCloth dDataCloth,
    DeviceSimParams dSimParam,
    int64_t nParticles);

__global__ void keComputeStretchCloth(
    DeviceDataCloth dDataCloth,
    DeviceSimParams dSimParam,
    int64_t nStretchLine);

__global__ void keComputeBendCloth(
    DeviceDataCloth dDataCloth,
    DeviceSimParams dSimParam,
    int64_t nStretchLine);

__global__ void keComputeShearCloth(
    DeviceDataCloth dDataCloth,
    DeviceSimParams dSimParam,
    int64_t nStretchLine);

__global__ void keUpdateCorretedPositionCloth(
    DeviceDataCloth dDataCloth,
    DeviceSimParams dSimParam,
    int64_t nParticles);

__global__ void keUpdateVelPosCloth(
    DeviceDataCloth dDataCloth,
    DeviceSimParams dSimParam,
    int64_t nParticles);

__global__ void keGetRenderValuesCloth(
    DeviceDataCloth dDataCloth,
    int64_t nParticles);

#endif // __HIPHYSICSPBD_H__