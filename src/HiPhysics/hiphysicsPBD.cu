#include "hiphysicsPBD.h"

#define PI  3.1415926535897932f
#define iPI 0.3183098861837906f
// __device__ void WendlandKernel(glm::vec3	dr)
// {
	
// }

__device__ float Poly6Kernel(float	H, float	R)
{
	float iH = 1.0f/H;
	//    res = 315    /(    64    *  PI *    H^9  ) * pow((H*H - R*R),3);
	float res = 315.0f * 0.015625f * iPI * pow(iH,9) * pow((H*H - R*R),3);
	if (R >= H) res = 0.0f;
	return res;
}

__device__ glm::vec3 SpikyGradKernel(float H, glm::vec3 dR)
{
	float iH = 1.0f/H;
	float R = length(dR);
	float iR = 1.0f/R;
	
	//    res = 45    /(   PI *    H^6  ) * pow((H - |dR|),2) dR / |dR|;
	glm::vec3 res = - 45.0f * iPI * powf(iH, 6) * powf((H - R), 2) * iR * dR;
	if (R >= H) res = glm::vec3(0.0f);
	if (R < 0.0001f) res = glm::vec3(0.0f);

	return res;
}

__global__ void keGetRenderValues(DeviceDataFluid dDataFluid,
								int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		// dDataFluid.colorValues[idx] = length(dDataFluid.velocities[idx]);
		// dDataFluid.colorValues[idx] = static_cast<float>(dDataFluid.gridIndices[idx]);
		// dDataFluid.colorValues[idx] = dDataFluid.constraints[idx];
		dDataFluid.colorValues[idx] = dDataFluid.lambdas[idx];
		// dDataFluid.colorValues[idx] = length(dDataFluid.DeviceDataFluid[idx]);
	}
}

__global__ void keComputeGridID(DeviceDataFluid dDataFluid,
								glm::vec3 	v3MinPosition, 
								glm::vec3 	v3MaxPosition,
								int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{

		//TODO: compute outside of kernel function ;;;
		// float H = dDataFluid.commonParam->radius * 1.2f * 2.0f * 2.0f;
		float H = dDataFluid.commonParam->H;
		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dDataFluid.commonParam->radius) - v3MinPosition.x)/H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dDataFluid.commonParam->radius) - v3MinPosition.y)/H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dDataFluid.commonParam->radius) - v3MinPosition.z)/H)+1;

		int32_t tmpInt = static_cast<int32_t>((dDataFluid.correctedPos[idx].x - v3MinPosition.x -(dDataFluid.commonParam->radius))/H)
					+ (ix)*static_cast<int32_t>((dDataFluid.correctedPos[idx].z - v3MinPosition.z -(dDataFluid.commonParam->radius))/H)
					+ (ix)*(iz)*static_cast<int32_t>((dDataFluid.correctedPos[idx].y - v3MinPosition.y -(dDataFluid.commonParam->radius))/H);
		dDataFluid.gridIndices[idx] = tmpInt;
	}
}


__global__ void keCountParticlesInGrids(DeviceDataFluid dDataFluid,
								glm::vec3 	v3MinPosition, 
								glm::vec3 	v3MaxPosition,
								int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{

		//TODO: compute outside of kernel function ;;;
		float H = dDataFluid.commonParam->radius * 1.2f * 2.0f * 2.0f;
		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dDataFluid.commonParam->radius) - v3MinPosition.x)/H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dDataFluid.commonParam->radius) - v3MinPosition.y)/H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dDataFluid.commonParam->radius) - v3MinPosition.z)/H)+1;

		int32_t tmpInt = static_cast<int32_t>((dDataFluid.correctedPos[idx].x - v3MinPosition.x -(dDataFluid.commonParam->radius))/H)
					+ (ix)*static_cast<int32_t>((dDataFluid.correctedPos[idx].z - v3MinPosition.z -(dDataFluid.commonParam->radius))/H)
					+ (ix)*(iz)*static_cast<int32_t>((dDataFluid.correctedPos[idx].y - v3MinPosition.y -(dDataFluid.commonParam->radius))/H);
		atomicAdd(&dDataFluid.numPartInGrids[tmpInt], 1);
	}
}

/*
keComputeConstraint
1. 로컬 변수 정의
2. 그리드 순회
	3. 그리드 내 입자 순회
		4. 예외처리
		5. 실제물리식 (1에서 정의된 변수도 사용함)
6. 글로벌 변수 업데이트
 
 // Lambda 함수 시급
kernelVariable kv;
for_NearParticles(DeviceDataFluid dDataFluid, FunctionPointer PhysicsComputation(KV));

*/
// struct kernelVariables;
// __device__ void for_NearParticles(DeviceDataFluid dDataFluid, FunctionPointer Computeconstraint(kernelVariables KV))
// {
// 	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
// 	int32_t IID = idx;
// 	int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dDataFluid.commonParam->radius) - v3MinPosition.x)/KV.H)+1;
// 	int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dDataFluid.commonParam->radius) - v3MinPosition.y)/KV.H)+1;
// 	int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dDataFluid.commonParam->radius) - v3MinPosition.z)/KV.H)+1;
// 	for (int32_t yyy = -1 ; yyy < 2  ; ++yyy)
// 		for (int32_t zzz = -1 ; zzz < 2  ; ++zzz)
// 			for (int32_t xxx = -1 ; xxx < 2  ; ++xxx)
// 			{
// 				int32_t nearGridID = dDataFluid.gridIndices[idx] + xxx + ix*zzz + ix*iz*yyy;
// 				if ( (nearGridID < 0) || (nearGridID > ix*iy*iz-1) ) continue;
// 				int32_t staJID = nearGridID == 0 ? 0 : dDataFluid.numPartInGrids[nearGridID-1];
// 				int32_t endJID = dDataFluid.numPartInGrids[nearGridID];
// 				for (int32_t JID = staJID; JID < endJID; ++JID)
// 				{
					
// 				}
// 			}

// 	KV.gradConstraintSqrSum += dot(KV.gradConstraintI,KV.gradConstraintI);
// 	KV.constraintI = KV.densityI*KV.iDensityI0 - 1.0f;
// }
struct kernelVariables {
	int32_t IID;
	float densityI0;
	float iDensityI0;
	float H;
};

struct computeConstraintKernelVariables : kernelVariables {
	float densityI = 0.0f;
	float constraintI = 0.0f;
	glm::vec3 gradConstraintI = glm::vec3(0.0f);
	float gradConstraintSqrSum = 0.0f;
};

inline __device__ void ComputeConstraint(int32_t &JID, 
									DeviceDataFluid &dDataFluid,
									computeConstraintKernelVariables &KV)
{
	int32_t IID = KV.IID;
	glm::vec3 displaceVectorIJ = dDataFluid.correctedPos[IID] - dDataFluid.correctedPos[JID];
	float distanceIJ = sqrt(glm::dot(displaceVectorIJ, displaceVectorIJ));

	if (distanceIJ < (KV.H * 0.5f))
	{
		float particleVolume = pow(2.0f * dDataFluid.commonParam->radius, 3);
		float kernelWeight = Poly6Kernel(0.5f * KV.H, distanceIJ);
		float densityJ0 = dDataFluid.phaseParam[dDataFluid.phases[JID]].density;

		KV.densityI += densityJ0 * particleVolume * kernelWeight;

		if (IID == JID) return;
		if (distanceIJ < KV.H * 0.00001f) return;

		glm::vec3 gradKernelWeight = SpikyGradKernel(0.5f * KV.H, displaceVectorIJ);
		glm::vec3 gradConstraintIJ = KV.iDensityI0 * densityJ0 * particleVolume * gradKernelWeight;
		KV.gradConstraintI += gradConstraintIJ;
		KV.gradConstraintSqrSum += dot(-gradConstraintIJ, -gradConstraintIJ);
	}
}

inline __device__ void ComputeConstraintToGlobal(DeviceDataFluid &dDataFluid,
										computeConstraintKernelVariables &KV)
{
	KV.gradConstraintSqrSum += dot(KV.gradConstraintI,KV.gradConstraintI);
	KV.constraintI = KV.densityI*KV.iDensityI0 - 1.0f;
	dDataFluid.lambdas[KV.IID] = - KV.constraintI / (KV.gradConstraintSqrSum + dDataFluid.commonParam->relaxationParameter);
}

__global__ void keComputeConstraint(DeviceDataFluid dDataFluid,
									glm::vec3 	v3MinPosition, 
									glm::vec3 	v3MaxPosition,
									int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		// KernelVariables KV; 
		// for_NearParticles(ComputeConstraint, dDataFluid, KV); ==> 안에서 IID는 고정, JID는 내부에서 계산
		// dDataFluid.update;

		computeConstraintKernelVariables KV; 
		KV.IID					= idx;
		KV.densityI0			= dDataFluid.phaseParam[dDataFluid.phases[KV.IID]].density;
		KV.iDensityI0			= 1.0f/dDataFluid.phaseParam[dDataFluid.phases[KV.IID]].density;
		KV.H					= dDataFluid.commonParam->radius * 1.2f * 2.0f * 2.0f;

		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dDataFluid.commonParam->radius) - v3MinPosition.x)/KV.H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dDataFluid.commonParam->radius) - v3MinPosition.y)/KV.H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dDataFluid.commonParam->radius) - v3MinPosition.z)/KV.H)+1;
		for (int32_t yyy = -1 ; yyy < 2  ; ++yyy)
			for (int32_t zzz = -1 ; zzz < 2  ; ++zzz)
				for (int32_t xxx = -1 ; xxx < 2  ; ++xxx)
				{
					int32_t nearGridID = dDataFluid.gridIndices[idx] + xxx + ix*zzz + ix*iz*yyy;
					if ( (nearGridID < 0) || (nearGridID > ix*iy*iz-1) ) continue;
					int32_t staJID = nearGridID == 0 ? 0 : dDataFluid.numPartInGrids[nearGridID-1];
					int32_t endJID = dDataFluid.numPartInGrids[nearGridID];
					for (int32_t JID = staJID; JID < endJID; ++JID)
					{
						// 모두 이런 형식일 것이므로!
						ComputeConstraint(JID, dDataFluid, KV);
					}
				}
		ComputeConstraintToGlobal(dDataFluid, KV);
	}
}

__global__ void keComputePositionCorrection(DeviceDataFluid dDataFluid,
											glm::vec3 	v3MinPosition, 
											glm::vec3 	v3MaxPosition,
											int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	int32_t IID = idx;

	if(idx < nParticles)
	{
		float density0 = dDataFluid.phaseParam[dDataFluid.phases[IID]].density;
		float iDensity0 = 1.0f/density0;
		dDataFluid.deltaPos[IID] = glm::vec3(0.0f);
		float H = dDataFluid.commonParam->radius * 1.2f * 2.0f * 2.0f;
		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dDataFluid.commonParam->radius) - v3MinPosition.x)/H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dDataFluid.commonParam->radius) - v3MinPosition.y)/H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dDataFluid.commonParam->radius) - v3MinPosition.z)/H)+1;
		
		for (int32_t yyy = -1 ; yyy < 2  ; ++yyy)
			for (int32_t zzz = -1 ; zzz < 2  ; ++zzz)
				for (int32_t xxx = -1 ; xxx < 2  ; ++xxx)
				{
					int32_t nearGridID = dDataFluid.gridIndices[idx] + xxx + ix*zzz + ix*iz*yyy;
					if ( (nearGridID < 0) || (nearGridID > ix*iy*iz-1) ) continue;
					int32_t staJID = nearGridID == 0 ? 0 : dDataFluid.numPartInGrids[nearGridID-1];
					int32_t endJID = dDataFluid.numPartInGrids[nearGridID];
					for (int32_t JID = staJID; JID < endJID; ++JID)
					{
						glm::vec3 dr = dDataFluid.correctedPos[IID] - dDataFluid.correctedPos[JID];
						float dr2  = glm::dot(dr,dr);
						if ( dr2 < (H*H*0.25f) )
						{
							if (IID == JID) continue;
							if ( dr2 < (H*H*0.0000001f) ) continue;

							float volume = pow(2.0f*dDataFluid.commonParam->radius,3);
							float dlen   = sqrt(dr2);
							glm::vec3 gradKernel = SpikyGradKernel(0.5f*H, dr);
							
							float scorr = - dDataFluid.commonParam->scorrK * powf(Poly6Kernel(0.5f*H, dlen) / Poly6Kernel(0.5f*H, 0.5f*H*dDataFluid.commonParam->scorrDq),4.0f);

							dDataFluid.deltaPos[IID] += iDensity0 * ((dDataFluid.lambdas[IID] + dDataFluid.lambdas[JID])*0.5f + scorr) * dDataFluid.phaseParam[dDataFluid.phases[JID]].density * volume * gradKernel;							
						}
					}
				}
		// bool check = false;
		// if (dDataFluid.correctedPos[IID].x < dDataFluid.commonParam->AnalysisBox.minPoint.x)  check = true;
		// if (dDataFluid.correctedPos[IID].x > dDataFluid.commonParam->AnalysisBox.maxPoint.x)  check = true;
		// if (dDataFluid.correctedPos[IID].y < dDataFluid.commonParam->AnalysisBox.minPoint.y)  check = true;
		// if (dDataFluid.correctedPos[IID].y > dDataFluid.commonParam->AnalysisBox.maxPoint.y)  check = true;
		// if (dDataFluid.correctedPos[IID].z < dDataFluid.commonParam->AnalysisBox.minPoint.z)  check = true;
		// if (dDataFluid.correctedPos[IID].z > dDataFluid.commonParam->AnalysisBox.maxPoint.z)  check = true;
		// dDataFluid.deltaPos[IID] = -0.01f*dDataFluid.deltaPos[IID];
	}
}

__global__ void kePredictPosition(DeviceDataFluid dDataFluid, 
						 		int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		dDataFluid.velocities[idx] += dDataFluid.commonParam->dt * dDataFluid.commonParam->gravity;
		dDataFluid.correctedPos[idx] = dDataFluid.positions[idx] + dDataFluid.commonParam->dt*dDataFluid.velocities[idx];
	}
}

__global__ void keUpdateCorretedPosition(DeviceDataFluid dDataFluid, 
						 				int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		dDataFluid.correctedPos[idx] = dDataFluid.correctedPos[idx] + dDataFluid.deltaPos[idx];
		
		if (dDataFluid.correctedPos[idx].x < dDataFluid.commonParam->AnalysisBox.minPoint.x + dDataFluid.commonParam->radius) dDataFluid.correctedPos[idx].x = dDataFluid.commonParam->AnalysisBox.minPoint.x + dDataFluid.commonParam->radius;
		if (dDataFluid.correctedPos[idx].x > dDataFluid.commonParam->AnalysisBox.maxPoint.x - dDataFluid.commonParam->radius) dDataFluid.correctedPos[idx].x = dDataFluid.commonParam->AnalysisBox.maxPoint.x - dDataFluid.commonParam->radius;
		if (dDataFluid.correctedPos[idx].y < dDataFluid.commonParam->AnalysisBox.minPoint.y + dDataFluid.commonParam->radius) dDataFluid.correctedPos[idx].y = dDataFluid.commonParam->AnalysisBox.minPoint.y + dDataFluid.commonParam->radius;
		if (dDataFluid.correctedPos[idx].y > dDataFluid.commonParam->AnalysisBox.maxPoint.y - dDataFluid.commonParam->radius) dDataFluid.correctedPos[idx].y = dDataFluid.commonParam->AnalysisBox.maxPoint.y - dDataFluid.commonParam->radius;
		if (dDataFluid.correctedPos[idx].z < dDataFluid.commonParam->AnalysisBox.minPoint.z + dDataFluid.commonParam->radius) dDataFluid.correctedPos[idx].z = dDataFluid.commonParam->AnalysisBox.minPoint.z + dDataFluid.commonParam->radius;
		if (dDataFluid.correctedPos[idx].z > dDataFluid.commonParam->AnalysisBox.maxPoint.z - dDataFluid.commonParam->radius) dDataFluid.correctedPos[idx].z = dDataFluid.commonParam->AnalysisBox.maxPoint.z - dDataFluid.commonParam->radius;
	}
}

__global__ void keUpdateVelPos(DeviceDataFluid dDataFluid, 
						 		int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		dDataFluid.velocities[idx] = (dDataFluid.correctedPos[idx] - dDataFluid.positions[idx])/dDataFluid.commonParam->dt;
		dDataFluid.positions[idx]  =  dDataFluid.correctedPos[idx];
		
		if (dDataFluid.positions[idx].x < dDataFluid.commonParam->AnalysisBox.minPoint.x + dDataFluid.commonParam->radius) dDataFluid.positions[idx].x = dDataFluid.commonParam->AnalysisBox.minPoint.x + dDataFluid.commonParam->radius;
		if (dDataFluid.positions[idx].x > dDataFluid.commonParam->AnalysisBox.maxPoint.x - dDataFluid.commonParam->radius) dDataFluid.positions[idx].x = dDataFluid.commonParam->AnalysisBox.maxPoint.x - dDataFluid.commonParam->radius;
		if (dDataFluid.positions[idx].y < dDataFluid.commonParam->AnalysisBox.minPoint.y + dDataFluid.commonParam->radius) dDataFluid.positions[idx].y = dDataFluid.commonParam->AnalysisBox.minPoint.y + dDataFluid.commonParam->radius;
		if (dDataFluid.positions[idx].y > dDataFluid.commonParam->AnalysisBox.maxPoint.y - dDataFluid.commonParam->radius) dDataFluid.positions[idx].y = dDataFluid.commonParam->AnalysisBox.maxPoint.y - dDataFluid.commonParam->radius;
		if (dDataFluid.positions[idx].z < dDataFluid.commonParam->AnalysisBox.minPoint.z + dDataFluid.commonParam->radius) dDataFluid.positions[idx].z = dDataFluid.commonParam->AnalysisBox.minPoint.z + dDataFluid.commonParam->radius;
		if (dDataFluid.positions[idx].z > dDataFluid.commonParam->AnalysisBox.maxPoint.z - dDataFluid.commonParam->radius) dDataFluid.positions[idx].z = dDataFluid.commonParam->AnalysisBox.maxPoint.z - dDataFluid.commonParam->radius;
	}
}

__global__ void kePredictPositionCloth(DeviceDataCloth dDataCloth, 
								DeviceSimParams dSimParam,
						 		int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	
	if(idx < nParticles)
	{
		if ((idx == 0) || (idx == 13))
			dDataCloth.velocities[idx] += glm::vec3(0.0f);
		else
			dDataCloth.velocities[idx] += dSimParam.commonParam->dt * dSimParam.commonParam->gravity;

		dDataCloth.correctedPos[idx] = dDataCloth.positions[idx] + dSimParam.commonParam->dt*dDataCloth.velocities[idx];

		// if (idx == 0)
		// {
		// 	dDataCloth.velocities[idx]  = glm::vec3(0.0);
		// 	dDataCloth.correctedPos[idx]= dDataCloth.positions[idx];
		// }
	}
}

__global__ void keComputeStretchCloth(DeviceDataCloth dDataCloth,
    								DeviceSimParams dSimParam,
									int64_t 	nStretchLines)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;

	if(idx < nStretchLines)
	{
		int32_t id0 = dDataCloth.stretchID[2*idx];
		int32_t id1 = dDataCloth.stretchID[2*idx + 1];

        glm::vec3 p0 = dDataCloth.correctedPos[id0];
        glm::vec3 p1 = dDataCloth.correctedPos[id1];
        
        glm::vec3 d = p1 - p0;
		glm::vec3 norm = glm::normalize(d);
		float len = glm::length(d);
		float len0= 2.0f * dSimParam.commonParam->radius;

        glm::vec3 dP = norm * 0.2f * (len - len0);
		
		if ((id0 == 0) || (id0 == 13))
		{

		}
		else
		{
			atomicAdd(&dDataCloth.deltaPos[id0].x, dP.x);
			atomicAdd(&dDataCloth.deltaPos[id0].y, dP.y);
			atomicAdd(&dDataCloth.deltaPos[id0].z, dP.z);	
		}

		if ((id1 == 0) || (id1 == 13))
		{
			
		}
		else
		{
			atomicAdd(&dDataCloth.deltaPos[id1].y, -dP.y);
			atomicAdd(&dDataCloth.deltaPos[id1].x, -dP.x);
			atomicAdd(&dDataCloth.deltaPos[id1].z, -dP.z);
		}


	}
}


__global__ void keComputeBendCloth(DeviceDataCloth dDataCloth,
    								DeviceSimParams dSimParam,
									int64_t 	nBendLines)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nBendLines)
	{

		int32_t id0 = dDataCloth.bendID[2*idx];
		int32_t id1 = dDataCloth.bendID[2*idx + 1];

        glm::vec3 p0 = dDataCloth.correctedPos[id0];
        glm::vec3 p1 = dDataCloth.correctedPos[id1];
        
        glm::vec3 d = p1 - p0;
		glm::vec3 norm = glm::normalize(d);
		float len = glm::length(d);
		float len0= 4.0f * dSimParam.commonParam->radius;


        glm::vec3 dP = norm * 0.2f * (len - len0);
		
		if ((id0 == 0) || (id0 == 13))
		{

		}
		else
		{
			atomicAdd(&dDataCloth.deltaPos[id0].x, dP.x);
			atomicAdd(&dDataCloth.deltaPos[id0].y, dP.y);
			atomicAdd(&dDataCloth.deltaPos[id0].z, dP.z);	
		}

		if ((id1 == 0) || (id1 == 13))
		{
			
		}
		else
		{
			atomicAdd(&dDataCloth.deltaPos[id1].y, -dP.y);
			atomicAdd(&dDataCloth.deltaPos[id1].x, -dP.x);
			atomicAdd(&dDataCloth.deltaPos[id1].z, -dP.z);
		}
	}
}


__global__ void keComputeShearCloth(DeviceDataCloth dDataCloth,
    								DeviceSimParams dSimParam,
									int64_t 	nShearLines)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nShearLines)
	{

		int32_t id0 = dDataCloth.shearID[2*idx];
		int32_t id1 = dDataCloth.shearID[2*idx + 1];

        glm::vec3 p0 = dDataCloth.correctedPos[id0];
        glm::vec3 p1 = dDataCloth.correctedPos[id1];
        
        glm::vec3 d = p1 - p0;
		glm::vec3 norm = glm::normalize(d);
		float len = glm::length(d);
		float len0= sqrt(2.0f) * 2.0f * dSimParam.commonParam->radius;

        glm::vec3 dP = norm * 0.2f * (len - len0);
		
		if ((id0 == 0) || (id0 == 13))
		{

		}
		else
		{
			atomicAdd(&dDataCloth.deltaPos[id0].x, dP.x);
			atomicAdd(&dDataCloth.deltaPos[id0].y, dP.y);
			atomicAdd(&dDataCloth.deltaPos[id0].z, dP.z);	
		}

		if ((id1 == 0) || (id1 == 13))
		{
			
		}
		else
		{
			atomicAdd(&dDataCloth.deltaPos[id1].y, -dP.y);
			atomicAdd(&dDataCloth.deltaPos[id1].x, -dP.x);
			atomicAdd(&dDataCloth.deltaPos[id1].z, -dP.z);
		}
	}
}

__global__ void keUpdateCorretedPositionCloth(DeviceDataCloth dDataCloth, DeviceSimParams dSimParam, int64_t nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		dDataCloth.correctedPos[idx] = dDataCloth.correctedPos[idx] + dDataCloth.deltaPos[idx];
		dDataCloth.deltaPos[idx] = glm::vec3(0.0f);
	}
}

__global__ void keUpdateVelPosCloth(DeviceDataCloth dDataCloth, 
    								DeviceSimParams dSimParam,
						 			int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		// if (idx == 0) return;
		dDataCloth.velocities[idx] = (dDataCloth.correctedPos[idx] - dDataCloth.positions[idx])/dSimParam.commonParam->dt;
		dDataCloth.positions[idx]  =  dDataCloth.correctedPos[idx];
	}
}


__global__ void keGetRenderValuesCloth(DeviceDataCloth dDataCloth,
								int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		// dDataCloth.colorValues[idx] = length(dDataCloth.velocities[idx]);
		// dDataCloth.colorValues[idx] = static_cast<float>(dDataCloth.gridIndices[idx]);
		// dDataCloth.colorValues[idx] = dDataCloth.constraints[idx];
		dDataCloth.colorValues[idx] = dDataCloth.velocities[idx].x;
		// dDataCloth.colorValues[idx] = length(dDataCloth.DeviceDataFluid[idx]);
	}
}
