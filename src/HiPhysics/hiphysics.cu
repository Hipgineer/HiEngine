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

    cudaFree(dm_Data.colorValues);
    cudaFree(dm_Data.positions);
    cudaFree(dm_Data.velocities);
    cudaFree(dm_Data.phases);
    cudaFree(dm_Data.constraints);
    cudaFree(dm_Data.lambdas);
    cudaFree(dm_Data.correctedPos);
    cudaFree(dm_Data.deltaPos);
    cudaFree(dm_Data.gridIndices);
    cudaFree(dm_Data.numPartInGrids);
    cudaFree(dm_Data.nearGridID);
    cudaFree(dm_Data.commonParam);
    cudaFree(dm_Data.phaseParam);

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
    cudaMalloc(&(dm_Data.colorValues), count*sizeof(float));
    cudaMemset(dm_Data.colorValues, 0, count*sizeof(float));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.colorValues %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.positions, count*sizeof(glm::vec3));
	cudaMemcpy(dm_Data.positions, simBuffer->m_positions.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.positions %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.velocities, count*sizeof(glm::vec3));
	cudaMemcpy(dm_Data.velocities, simBuffer->m_velocities.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.velocities %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.phases, count*sizeof(int32_t));
	cudaMemcpy(dm_Data.phases, simBuffer->m_phases.data(), count*sizeof(int32_t), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.phases %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.constraints, count*sizeof(float));
    cudaMemset(dm_Data.constraints, 0, count*sizeof(float));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.constraints %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.lambdas, count*sizeof(float));
    cudaMemset(dm_Data.lambdas, 0, count*sizeof(float));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.lambdas %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.correctedPos, count*sizeof(glm::vec3));
    cudaMemset(dm_Data.correctedPos, 0, count*sizeof(glm::vec3));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.correctedPos %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.deltaPos, count*sizeof(glm::vec3));
    cudaMemset(dm_Data.deltaPos, 0, count*sizeof(glm::vec3));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.deltaPos %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.gridIndices, count*sizeof(int32_t));
    cudaMemset(dm_Data.gridIndices, 0, count*sizeof(int32_t));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.gridIndices %s\n",cudaGetErrorString(cudaError));
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
    cudaMalloc(&dm_Data.numPartInGrids, 1000*(ix)*(iy)*(iz)*sizeof(int32_t));
    cudaMemset(dm_Data.numPartInGrids, 0, 1000*(ix)*(iy)*(iz)*sizeof(int32_t));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.numPartInGrids %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
    cudaMalloc(&dm_Data.nearGridID, 3*(ix+1)*(iy+1)*(iz+1)*27*sizeof(int32_t));
    cudaMemset(dm_Data.nearGridID, 0, 3*(ix+1)*(iy+1)*(iz+1)*27*sizeof(int32_t));
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.nearGridID %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.commonParam, sizeof(CommonParameters));
	cudaMemcpy(dm_Data.commonParam, &simBuffer->m_commonParam, sizeof(CommonParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.commonParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    cudaMalloc(&dm_Data.phaseParam, simBuffer->m_phaseParam.size()*sizeof(PhaseParameters));
	cudaMemcpy(dm_Data.phaseParam, simBuffer->m_phaseParam.data(), simBuffer->m_phaseParam.size()*sizeof(PhaseParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("MallocMemcpy dm_Data.phaseParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::GetMemory(SimBufferPtr simBuffer) {
    cudaError_t cudaError;

    uint64_t count = simBuffer->GetNumParticles();

	cudaMemcpy(simBuffer->m_colorValues.data(), dm_Data.colorValues, count*sizeof(float),    cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_positions.data(),   dm_Data.positions,   count*sizeof(glm::vec3),cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_velocities.data(),  dm_Data.velocities,  count*sizeof(glm::vec3),cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize(); cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Error at HiPhysics::GetMemory %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}
	cudaMemcpy(simBuffer->m_phases.data(),      dm_Data.phases,      count*sizeof(int32_t),  cudaMemcpyDeviceToHost);
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

	// cudaMemcpy(dm_Data.positions, simBuffer->m_positions.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	// cudaDeviceSynchronize(); 
    // cudaError = cudaGetLastError();
	// if (cudaError != cudaSuccess)
  	// {
    //     printf("Memcpy dm_Data.positions %s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    //     return false;
  	// }

	// cudaMemcpy(dm_Data.velocities, simBuffer->m_velocities.data(), count*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	// cudaDeviceSynchronize(); 
    // cudaError = cudaGetLastError();
	// if (cudaError != cudaSuccess)
  	// {
    //     printf("Memcpy dm_Data.velocities %s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    //     return false;
  	// }

	// cudaMemcpy(dm_Data.phases, simBuffer->m_phases.data(), count*sizeof(int32_t), cudaMemcpyHostToDevice);
	// cudaDeviceSynchronize(); 
    // cudaError = cudaGetLastError();
	// if (cudaError != cudaSuccess)
  	// {
    //     printf("Memcpy dm_Data.phases %s\n",cudaGetErrorString(cudaError));
    //     exit(1);
    //     return false;
  	// }
    
	cudaMemcpy(dm_Data.commonParam, &simBuffer->m_commonParam, sizeof(CommonParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Memcpy dm_Data.commonParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

	cudaMemcpy(dm_Data.phaseParam, simBuffer->m_phaseParam.data(), simBuffer->m_phaseParam.size()*sizeof(PhaseParameters), cudaMemcpyHostToDevice);
	cudaDeviceSynchronize(); 
    cudaError = cudaGetLastError();
	if (cudaError != cudaSuccess)
  	{
        printf("Memcpy dm_Data.phaseParam %s\n",cudaGetErrorString(cudaError));
        exit(1);
        return false;
  	}

    return true;
}

bool HiPhysics::PredictPosition(SimBufferPtr simBuffer) {

    cudaError_t cudaError; // TODO : make it as a member variable.

    // 0. Predict Position
    kePredictPosition<<< 1 +  m_numParticles/256, 256>>>(dm_Data, m_numParticles);

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
    cudaMemset(dm_Data.numPartInGrids, 0, (ix)*(iy)*(iz)*sizeof(int32_t));
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("clear(Memset) dm_Data.numPartInGrids %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // 1. assign Grid ID to Particles.
    keComputeGridID<<< 1 +  m_numParticles/256, 256>>>(dm_Data, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::keComputeGridID %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();


    // 2. Count the number of Particles in each Grids.
    keCountParticlesInGrids<<< 1 +  m_numParticles/256, 256>>>(dm_Data, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::keCountParticlesInGrids %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // 3. Inclusive scan the number of particels in each Grids.
    {
        thrust::device_vector<int32_t> temp(dm_Data.numPartInGrids,dm_Data.numPartInGrids + ix*iy*iz);
        thrust::device_ptr<int32_t> dev_ptr = thrust::device_pointer_cast(dm_Data.numPartInGrids);
        thrust::inclusive_scan(temp.begin(), temp.end(), dev_ptr);
    }

    return true;
}

bool HiPhysics::SortVariablesByIndices(SimBufferPtr simBuffer) {
    
    thrust::device_vector<int> indices(m_numParticles); 
    thrust::sequence(indices.begin(),indices.end());
    thrust::sort_by_key(dm_Data.gridIndices,dm_Data.gridIndices+m_numParticles,indices.begin());

    {
        thrust::device_ptr<float> dev_ptr = thrust::device_pointer_cast(dm_Data.colorValues);
        thrust::device_vector<float> temp(dm_Data.colorValues,dm_Data.colorValues+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_Data.positions);
        thrust::device_vector<glm::vec3> temp(dm_Data.positions,dm_Data.positions+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }
    
    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_Data.velocities);
        thrust::device_vector<glm::vec3> temp(dm_Data.velocities,dm_Data.velocities+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<int32_t> dev_ptr = thrust::device_pointer_cast(dm_Data.phases);
        thrust::device_vector<int32_t> temp(dm_Data.phases,dm_Data.phases+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<float> dev_ptr = thrust::device_pointer_cast(dm_Data.constraints);
        thrust::device_vector<float> temp(dm_Data.constraints,dm_Data.constraints+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<float> dev_ptr = thrust::device_pointer_cast(dm_Data.lambdas);
        thrust::device_vector<float> temp(dm_Data.lambdas,dm_Data.lambdas+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }
    
    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_Data.deltaPos);
        thrust::device_vector<glm::vec3> temp(dm_Data.deltaPos,dm_Data.deltaPos+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    {
        thrust::device_ptr<glm::vec3> dev_ptr = thrust::device_pointer_cast(dm_Data.correctedPos);
        thrust::device_vector<glm::vec3> temp(dm_Data.correctedPos,dm_Data.correctedPos+m_numParticles);
        thrust::gather(indices.begin(),indices.end(), temp.data(), dev_ptr);
    }

    return true;
}

bool HiPhysics::ComputeConstraint(SimBufferPtr simBuffer){

    cudaError_t cudaError;

    glm::vec3 maxPosition = max_element_xyz(&simBuffer->m_positions) + glm::vec3(simBuffer->m_commonParam.radius);
    glm::vec3 minPosition = min_element_xyz(&simBuffer->m_positions) - glm::vec3(simBuffer->m_commonParam.radius);

    // Compute Constraints
    keComputeConstraint<<< 1 +  m_numParticles/256, 256>>>(dm_Data, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keComputeConstraint  %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // Correct Positions
    keComputePositionCorrection<<< 1 +  m_numParticles/256, 256>>>(dm_Data, minPosition, maxPosition, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keComputePositionCorrection  %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

    // Update Corrected Positions
    keUpdateCorretedPosition<<< 1 +  m_numParticles/256, 256>>>(dm_Data, m_numParticles);
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysics::ComputeConstraint-keUpdatePosition :%s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();

	cudaMemcpy(simBuffer->m_positions.data(),      dm_Data.correctedPos,      m_numParticles*sizeof(glm::vec3),  cudaMemcpyDeviceToHost);
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

    keUpdateVelPos<<< 1 +  m_numParticles/256, 256>>>(dm_Data, m_numParticles);

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

    keGetRenderValues<<< 1 +  m_numParticles/256, 256>>>(dm_Data, m_numParticles);

    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess)
    {
        printf("Error at HiPhysicsPBD::keGetRenderValues %s\n",cudaGetErrorString(cudaError));
        exit(1);
    }
    cudaDeviceSynchronize();
    return true;
}