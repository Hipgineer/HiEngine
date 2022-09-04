#include "hiphysics.h"
#include "hiphysicsPBD.h"

HiPhysicsUPtr HiPhysics::Create() {
    auto solver = HiPhysicsUPtr(new HiPhysics());
    if(!solver->Init())
        return nullptr;
    return std::move(solver);
}

bool HiPhysics::ClearMemory() {

    cudaError_t cudaError;

    cudaFree(dm_DataFluid.colorValues);
    cudaFree(dm_DataFluid.positions);
    cudaFree(dm_DataFluid.velocities);
    cudaFree(dm_DataFluid.phases);
    cudaFree(dm_DataFluid.constraints);
    cudaFree(dm_DataFluid.lambdas);
    cudaFree(dm_DataFluid.correctedPos);
    cudaFree(dm_DataFluid.deltaPos);
    cudaFree(dm_DataFluid.gridIndices);
    cudaFree(dm_DataFluid.numPartInGrids);
    cudaFree(dm_DataFluid.nearGridID);
    cudaFree(dm_DataFluid.commonParam);
    cudaFree(dm_DataFluid.phaseParam);

    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Fail to HiPhysics::ClearMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
    return true;

}

bool HiPhysics::SetMemory(SimBufferPtr simBuffer) {
    cudaError_t cudaError;
    uint64_t count = simBuffer->GetNumParticles();

    //TODO : Dynamic allocation!
    // the number of particles is varying during the simulations!!
    cudaMalloc(&(dm_DataFluid.colorValues), count*sizeof(float));
    cudaMemset(dm_DataFluid.colorValues, 0, count*sizeof(float));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.colorValues %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.positions, count*sizeof(glm::vec3));
	cudaMemcpy(dm_DataFluid.positions, simBuffer->m_positions.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.positions %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.velocities, count*sizeof(glm::vec3));
	cudaMemcpy(dm_DataFluid.velocities, simBuffer->m_velocities.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.velocities %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.phases, count*sizeof(int32_t));
	cudaMemcpy(dm_DataFluid.phases, simBuffer->m_phases.data(), count*sizeof(int32_t), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.phases %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.constraints, count*sizeof(float));
    cudaMemset(dm_DataFluid.constraints, 0, count*sizeof(float));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.constraints %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.lambdas, count*sizeof(float));
    cudaMemset(dm_DataFluid.lambdas, 0, count*sizeof(float));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.lambdas %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.correctedPos, count*sizeof(glm::vec3));
    cudaMemset(dm_DataFluid.correctedPos, 0, count*sizeof(glm::vec3));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.correctedPos %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.deltaPos, count*sizeof(glm::vec3));
    cudaMemset(dm_DataFluid.deltaPos, 0, count*sizeof(glm::vec3));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.deltaPos %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.gridIndices, count*sizeof(int32_t));
    cudaMemset(dm_DataFluid.gridIndices, 0, count*sizeof(int32_t));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.gridIndices %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    // TODO
    glm::vec3 maxPosition = max_element_xyz(&simBuffer->m_positions) + glm::vec3(simBuffer->m_commonParam.radius);
    glm::vec3 minPosition = min_element_xyz(&simBuffer->m_positions) - glm::vec3(simBuffer->m_commonParam.radius);
    float H = simBuffer->m_commonParam.radius * 1.2f * 2.0f * 2.0f;
    int32_t ix = static_cast<int32_t>((maxPosition.x - (simBuffer->m_commonParam.radius) - minPosition.x)/H)+1;
    int32_t iy = static_cast<int32_t>((maxPosition.y - (simBuffer->m_commonParam.radius) - minPosition.y)/H)+1;
    int32_t iz = static_cast<int32_t>((maxPosition.z - (simBuffer->m_commonParam.radius) - minPosition.z)/H)+1;
    cudaMalloc(&dm_DataFluid.numPartInGrids, 1000*(ix)*(iy)*(iz)*sizeof(int32_t));
    cudaMemset(dm_DataFluid.numPartInGrids, 0, 1000*(ix)*(iy)*(iz)*sizeof(int32_t));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.numPartInGrids %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
    cudaMalloc(&dm_DataFluid.nearGridID, 3*(ix+1)*(iy+1)*(iz+1)*27*sizeof(int32_t));
    cudaMemset(dm_DataFluid.nearGridID, 0, 3*(ix+1)*(iy+1)*(iz+1)*27*sizeof(int32_t));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.nearGridID %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.commonParam, sizeof(CommonParameters));
	cudaMemcpy(dm_DataFluid.commonParam, &simBuffer->m_commonParam, sizeof(CommonParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.commonParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataFluid.phaseParam, simBuffer->m_phaseParam.size()*sizeof(PhaseParameters));
	cudaMemcpy(dm_DataFluid.phaseParam, simBuffer->m_phaseParam.data(), simBuffer->m_phaseParam.size()*sizeof(PhaseParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataFluid.phaseParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}


    cudaMalloc(&dm_SimParameters.commonParam, sizeof(CommonParameters));
	cudaMemcpy(dm_SimParameters.commonParam, &simBuffer->m_commonParam, sizeof(CommonParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_SimParameters.commonParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_SimParameters.phaseParam, simBuffer->m_phaseParam.size()*sizeof(PhaseParameters));
	cudaMemcpy(dm_SimParameters.phaseParam, simBuffer->m_phaseParam.data(), simBuffer->m_phaseParam.size()*sizeof(PhaseParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_SimParameters.phaseParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::SetMemoryCloth(SimBufferPtr simBuffer) {
    cudaError_t cudaError;
    uint64_t count = simBuffer->GetNumParticles();
    uint64_t nStretchLines = simBuffer->GetNumStretchLines();
    uint64_t nBendLines = simBuffer->GetNumBendLines();
    uint64_t nShearLines = simBuffer->GetNumShearLines();
    uint64_t nTriangles = simBuffer->GetNumTriangles();

    //TODO : Dynamic allocation!
    // the number of particles is varying during the simulations!!
    cudaMalloc(&(dm_DataCloth.colorValues), count*sizeof(float));
    cudaMemset(dm_DataCloth.colorValues, 0, count*sizeof(float));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.colorValues %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.positions, count*sizeof(glm::vec3));
	cudaMemcpy(dm_DataCloth.positions, simBuffer->m_positions.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.positions %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.velocities, count*sizeof(glm::vec3));
	cudaMemcpy(dm_DataCloth.velocities, simBuffer->m_velocities.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.velocities %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.phases, count*sizeof(int32_t));
	cudaMemcpy(dm_DataCloth.phases, simBuffer->m_phases.data(), count*sizeof(int32_t), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.phases %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.stretchID, 2*nStretchLines*sizeof(int32_t));
	cudaMemcpy(dm_DataCloth.stretchID, simBuffer->m_stretchID.data(), 2*nStretchLines*sizeof(int32_t), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.stretchID %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.bendID, 2*nBendLines*sizeof(int32_t));
    cudaMemcpy(dm_DataCloth.bendID, simBuffer->m_bendID.data(), 2*nBendLines*sizeof(int32_t), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.bendID %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.shearID, nShearLines*sizeof(int32_t));
    cudaMemcpy(dm_DataCloth.shearID, simBuffer->m_shearID.data(), nShearLines*sizeof(int32_t), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.shearID %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.correctedPos, count*sizeof(glm::vec3));
	cudaMemcpy(dm_DataCloth.correctedPos, simBuffer->m_positions.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.correctedPos %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_DataCloth.deltaPos, count*sizeof(glm::vec3));
    cudaMemset(dm_DataCloth.deltaPos, 0, count*sizeof(glm::vec3));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_DataCloth.deltaPos %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_SimParameters.commonParam, sizeof(CommonParameters));
	cudaMemcpy(dm_SimParameters.commonParam, &simBuffer->m_commonParam, sizeof(CommonParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_SimParameters.commonParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_SimParameters.phaseParam, simBuffer->m_phaseParam.size()*sizeof(PhaseParameters));
	cudaMemcpy(dm_SimParameters.phaseParam, simBuffer->m_phaseParam.data(), simBuffer->m_phaseParam.size()*sizeof(PhaseParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_SimParameters.phaseParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::GetMemory(SimBufferPtr simBuffer) {
    cudaError_t cudaError;

    uint64_t count = simBuffer->GetNumParticles();

	cudaMemcpy(simBuffer->m_colorValues.data(), dm_DataFluid.colorValues, count*sizeof(float),    cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_positions.data(),   dm_DataFluid.positions,   count*sizeof(glm::vec3),cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_velocities.data(),  dm_DataFluid.velocities,  count*sizeof(glm::vec3),cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_phases.data(),      dm_DataFluid.phases,      count*sizeof(int32_t),  cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::Init () {   
    // cudamemcopy

    return true;
}

void HiPhysics::UpdateSolver(SimBufferPtr simBuffer) {
    m_numParticles = simBuffer->GetNumParticles();
    
    if (m_numParticles > 0)
    {
        /// APPLY THE CHANGE BY USER INTERFACE
        MemsetFromHost(simBuffer);

        /// 
        PredictPosition(simBuffer);

        
        for (int32_t ii = 0; ii < simBuffer->m_commonParam.iterationNumber; ++ii)
        {
        /// COMPUTE GRID INDEX COUNT THE NUMBER OF PARTICLES IN THE GRID
            ComputeGridIndices(simBuffer);

        /// SORT BY GRID INDEX
            SortVariablesByIndices(simBuffer);

        /// COMPUTE CONSTRAINTS
            ComputeConstraint(simBuffer);
        }
            

        /// UPDATE PARTICLE POSITIONS
        UpdateVelPos(simBuffer);

        /// GET VALUES FOR RENDERING PARTICLE COLOR
        GetRenderingVariable(simBuffer);
    }
}

bool HiPhysics::MemsetFromHost(SimBufferPtr simBuffer) {
    cudaError_t cudaError;
    uint64_t count = simBuffer->GetNumParticles();

	// cudaMemcpy(dm_DataFluid.positions, simBuffer->m_positions.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	// cudaDeviceSynchronize(); 
    // cudaError = cudaGetLastError();
	// if (cudaError != cudaSuccess)
  	// {
    //     printf("Memcpy dm_DataFluid.positions %s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    //     return false;
  	// }

	// cudaMemcpy(dm_DataFluid.velocities, simBuffer->m_velocities.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	// cudaDeviceSynchronize(); 
    // cudaError = cudaGetLastError();
	// if (cudaError != cudaSuccess)
  	// {
    //     printf("Memcpy dm_DataFluid.velocities %s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    //     return false;
  	// }

	// cudaMemcpy(dm_DataFluid.phases, simBuffer->m_phases.data(), count*sizeof(int32_t), cudaMemcpyHostToDevice);
	// cudaDeviceSynchronize(); 
    // cudaError = cudaGetLastError();
	// if (cudaError != cudaSuccess)
  	// {
    //     printf("Memcpy dm_DataFluid.phases %s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    //     return false;
  	// }
    
	cudaMemcpy(dm_DataFluid.commonParam, &simBuffer->m_commonParam, sizeof(CommonParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Memcpy dm_DataFluid.commonParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

	cudaMemcpy(dm_DataFluid.phaseParam, simBuffer->m_phaseParam.data(), simBuffer->m_phaseParam.size()*sizeof(PhaseParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Memcpy dm_DataFluid.phaseParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::PredictPosition(SimBufferPtr simBuffer) {

    cudaError_t cudaError; // TODO : make it as a member variable.

    // 0. Predict Position
    kePredictPosition<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, m_numParticles);

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::kePredictPosition :%s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    return true;
}

bool HiPhysics::ComputeGridIndices(SimBufferPtr simBuffer){

    cudaError_t cudaError; // TODO : make it as a member variable.

    glm::vec3 maxPosition = max_element_xyz(&simBuffer->m_positions) + glm::vec3(simBuffer->m_commonParam.radius);
    glm::vec3 minPosition = min_element_xyz(&simBuffer->m_positions) - glm::vec3(simBuffer->m_commonParam.radius);
    float H = simBuffer->m_commonParam.radius * 1.2f * 2.0f * 2.0f;
    int32_t ix = static_cast<int32_t>((maxPosition.x - (simBuffer->m_commonParam.radius) - minPosition.x)/H)+1;
    int32_t iy = static_cast<int32_t>((maxPosition.y - (simBuffer->m_commonParam.radius) - minPosition.y)/H)+1;
    int32_t iz = static_cast<int32_t>((maxPosition.z - (simBuffer->m_commonParam.radius) - minPosition.z)/H)+1;
    cudaMemset(dm_DataFluid.numPartInGrids, 0, (ix)*(iy)*(iz)*sizeof(int32_t));
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("clear(Memset) dm_DataFluid.numPartInGrids %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // 1. assign Grid ID to Particles.
    keComputeGridID<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::keComputeGridID %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();


    // 2. Count the number of Particles in each Grids.
    keCountParticlesInGrids<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::keCountParticlesInGrids %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // 3. Inclusive scan the number of particels in each Grids.
    {
        thrust::device_vector<int32_t> temp(dm_DataFluid.numPartInGrids,dm_DataFluid.numPartInGrids + ix*iy*iz);
        thrust::device_ptr<int32_t> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.numPartInGrids);
        thrust::inclusive_scan(temp.begin(), temp.end(), dev_ptr);
    }

    return true;
}

bool HiPhysics::SortVariablesByIndices(SimBufferPtr simBuffer) {
    
    thrust::device_vector<int> indices(m_numParticles); 
    thrust::sequence(indices.begin(),indices.end());
    thrust::sort_by_key(dm_DataFluid.gridIndices,dm_DataFluid.gridIndices+m_numParticles,indices.begin());

    {
        thrust::device_ptr<float> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.colorValues);
        thrust::device_vector<float> temp(dm_DataFluid.colorValues,dm_DataFluid.colorValues+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.positions);
        thrust::device_vector<glm::vec3> temp(dm_DataFluid.positions,dm_DataFluid.positions+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }
    
    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.velocities);
        thrust::device_vector<glm::vec3> temp(dm_DataFluid.velocities,dm_DataFluid.velocities+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<int32_t> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.phases);
        thrust::device_vector<int32_t> temp(dm_DataFluid.phases,dm_DataFluid.phases+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<float> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.constraints);
        thrust::device_vector<float> temp(dm_DataFluid.constraints,dm_DataFluid.constraints+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<float> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.lambdas);
        thrust::device_vector<float> temp(dm_DataFluid.lambdas,dm_DataFluid.lambdas+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }
    
    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.deltaPos);
        thrust::device_vector<glm::vec3> temp(dm_DataFluid.deltaPos,dm_DataFluid.deltaPos+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_DataFluid.correctedPos);
        thrust::device_vector<glm::vec3> temp(dm_DataFluid.correctedPos,dm_DataFluid.correctedPos+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    return true;
}

bool HiPhysics::ComputeConstraint(SimBufferPtr simBuffer){

    cudaError_t cudaError;

    glm::vec3 maxPosition = max_element_xyz(&simBuffer->m_positions) + glm::vec3(simBuffer->m_commonParam.radius);
    glm::vec3 minPosition = min_element_xyz(&simBuffer->m_positions) - glm::vec3(simBuffer->m_commonParam.radius);

    // Compute Constraints
    keComputeConstraint<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keComputeConstraint  %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // Correct Positions
    keComputePositionCorrection<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keComputePositionCorrection  %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // Update Corrected Positions
    keUpdateCorretedPosition<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keUpdatePosition :%s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

	cudaMemcpy(simBuffer->m_positions.data(),      dm_DataFluid.correctedPos,      m_numParticles*sizeof(glm::vec3),  cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::ComputeConstraint-memcpyDelPos :%s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::UpdateVelPos(SimBufferPtr simBuffer){

    cudaError_t cudaError; // TODO : 맴버변수화 

    keUpdateVelPos<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, m_numParticles);

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::UpdateVelPos :%s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();
    return true;
}

bool HiPhysics::GetRenderingVariable(SimBufferPtr simBuffer){

    cudaError_t cudaError;

    keGetRenderValues<<< 1 +  m_numParticles/256, 256>>>(dm_DataFluid, m_numParticles);

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::keGetRenderValues %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();
    return true;
}










void HiPhysics::UpdateSolverCloth(SimBufferPtr simBuffer) {
    m_numParticles = simBuffer->GetNumParticles();
    
    if (m_numParticles > 0)
    {
        /// APPLY THE CHANGE BY USER INTERFACE
        MemsetFromHostCloth(simBuffer);

        /// 
        PredictPositionCloth(simBuffer);

        ///
        
        for (int32_t ii = 0; ii < simBuffer->m_commonParam.iterationNumber; ++ii)
        {
            ComputeConstraintCloth(simBuffer);
        }
            
        /// UPDATE PARTICLE POSITIONS
        UpdateVelPosCloth(simBuffer);

        /// GET VALUES FOR RENDERING PARTICLE COLOR
        GetRenderingVariableCloth(simBuffer);
    }
}


bool HiPhysics::MemsetFromHostCloth(SimBufferPtr simBuffer) {
    cudaError_t cudaError;
    uint64_t count = simBuffer->GetNumParticles();
    
	cudaMemcpy(dm_SimParameters.commonParam, &simBuffer->m_commonParam, sizeof(CommonParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Memcpy dm_SimParameters.commonParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

	cudaMemcpy(dm_SimParameters.phaseParam, simBuffer->m_phaseParam.data(), simBuffer->m_phaseParam.size()*sizeof(PhaseParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Memcpy dm_SimParameters.phaseParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::PredictPositionCloth(SimBufferPtr simBuffer) {

    cudaError_t cudaError; // TODO : make it as a member variable.

    // 0. Predict Position
    kePredictPositionCloth<<< 1 +  m_numParticles/256, 256>>>(dm_DataCloth, dm_SimParameters, m_numParticles);

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::kePredictPosition :%s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    return true;
}


bool HiPhysics::ComputeConstraintCloth(SimBufferPtr simBuffer){

    cudaError_t cudaError;
    // Compute Constraints
    keComputeStretchCloth<<< 1 +  simBuffer->GetNumStretchLines()/256, 256>>>(dm_DataCloth, dm_SimParameters, simBuffer->GetNumStretchLines());
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keComputeConstraintCloth  %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();
    
    keComputeBendCloth<<< 1 +  simBuffer->GetNumBendLines()/256, 256>>>(dm_DataCloth, dm_SimParameters, simBuffer->GetNumBendLines());
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keComputeConstraintCloth  %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();
    
    // keComputeShearCloth<<< 1 +  simBuffer->GetNumShearLines()/256, 256>>>(dm_DataCloth, dm_SimParameters, simBuffer->GetNumShearLines());
    // cudaError = cudaGetLastError();
    // if (cudaError != cudaSuccess)
    // {
    //     printf("Error at HiPhysics::ComputeConstraint-keComputeConstraintCloth  %s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    // }
    // cudaDeviceSynchronize();

    // Update Corrected Positions
    keUpdateCorretedPositionCloth<<< 1 +  m_numParticles/256, 256>>>(dm_DataCloth, dm_SimParameters, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keUpdateCorretedPositionCloth :%s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

	// cudaMemcpy(simBuffer->m_positions.data(),      dm_DataCloth.correctedPos,      m_numParticles*sizeof(glm::vec3),  cudaMemcpyDeviceToHost);
	// cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	// if (cudaError != cudaSuccess)
  	// {
    //     printf("Error at HiPhysics::ComputeConstraint-memcpyDelPos :%s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    //     return false;
  	// }

    return true;
}

bool HiPhysics::UpdateVelPosCloth(SimBufferPtr simBuffer){

    cudaError_t cudaError; // TODO : 맴버변수화 

    keUpdateVelPosCloth<<< 1 +  m_numParticles/256, 256>>>(dm_DataCloth, dm_SimParameters, m_numParticles);

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::UpdateVelPos :%s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();
    return true;
}

bool HiPhysics::GetRenderingVariableCloth(SimBufferPtr simBuffer){

    cudaError_t cudaError;

    keGetRenderValuesCloth<<< 1 +  m_numParticles/256, 256>>>(dm_DataCloth, m_numParticles);

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::keGetRenderValues %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();
    return true;
}



bool HiPhysics::GetMemoryCloth(SimBufferPtr simBuffer) {
    cudaError_t cudaError;

    uint64_t count = simBuffer->GetNumParticles();

	cudaMemcpy(simBuffer->m_colorValues.data(), dm_DataCloth.colorValues, count*sizeof(float),    cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_positions.data(),   dm_DataCloth.positions,   count*sizeof(glm::vec3),cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_velocities.data(),  dm_DataCloth.velocities,  count*sizeof(glm::vec3),cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_phases.data(),      dm_DataCloth.phases,      count*sizeof(int32_t),  cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}
