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

__global__ void keGetRenderValues(DeviceData dData,
								int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		// dData.colorValues[idx] = length(dData.velocities[idx]);
		// dData.colorValues[idx] = static_cast<float>(dData.gridIndices[idx]);
		// dData.colorValues[idx] = dData.constraints[idx];
		dData.colorValues[idx] = dData.lambdas[idx];
		// dData.colorValues[idx] = length(dData.deltaPos[idx]);
	}
}

__global__ void keComputeGridID(DeviceData dData,
								glm::vec3 	v3MinPosition, 
								glm::vec3 	v3MaxPosition,
								int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{

		//TODO: compute outside of kernel function ;;;
		// float H = dData.commonParam->radius * 1.2f * 2.0f * 2.0f;
		float H = dData.commonParam->H;
		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dData.commonParam->radius) - v3MinPosition.x)/H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dData.commonParam->radius) - v3MinPosition.y)/H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dData.commonParam->radius) - v3MinPosition.z)/H)+1;

		int32_t tmpInt = static_cast<int32_t>((dData.correctedPos[idx].x - v3MinPosition.x -(dData.commonParam->radius))/H)
					+ (ix)*static_cast<int32_t>((dData.correctedPos[idx].z - v3MinPosition.z -(dData.commonParam->radius))/H)
					+ (ix)*(iz)*static_cast<int32_t>((dData.correctedPos[idx].y - v3MinPosition.y -(dData.commonParam->radius))/H);
		dData.gridIndices[idx] = tmpInt;
	}
}


__global__ void keCountParticlesInGrids(DeviceData dData,
								glm::vec3 	v3MinPosition, 
								glm::vec3 	v3MaxPosition,
								int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{

		//TODO: compute outside of kernel function ;;;
		float H = dData.commonParam->radius * 1.2f * 2.0f * 2.0f;
		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dData.commonParam->radius) - v3MinPosition.x)/H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dData.commonParam->radius) - v3MinPosition.y)/H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dData.commonParam->radius) - v3MinPosition.z)/H)+1;

		int32_t tmpInt = static_cast<int32_t>((dData.correctedPos[idx].x - v3MinPosition.x -(dData.commonParam->radius))/H)
					+ (ix)*static_cast<int32_t>((dData.correctedPos[idx].z - v3MinPosition.z -(dData.commonParam->radius))/H)
					+ (ix)*(iz)*static_cast<int32_t>((dData.correctedPos[idx].y - v3MinPosition.y -(dData.commonParam->radius))/H);
		atomicAdd(&dData.numPartInGrids[tmpInt], 1);
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
for_NearParticles(DeviceData dData, FunctionPointer PhysicsComputation(KV));

*/
// struct kernelVariables;
// __device__ void for_NearParticles(DeviceData dData, FunctionPointer Computeconstraint(kernelVariables KV))
// {
// 	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
// 	int32_t IID = idx;
// 	int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dData.commonParam->radius) - v3MinPosition.x)/KV.H)+1;
// 	int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dData.commonParam->radius) - v3MinPosition.y)/KV.H)+1;
// 	int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dData.commonParam->radius) - v3MinPosition.z)/KV.H)+1;
// 	for (int32_t yyy = -1 ; yyy < 2  ; ++yyy)
// 		for (int32_t zzz = -1 ; zzz < 2  ; ++zzz)
// 			for (int32_t xxx = -1 ; xxx < 2  ; ++xxx)
// 			{
// 				int32_t nearGridID = dData.gridIndices[idx] + xxx + ix*zzz + ix*iz*yyy;
// 				if ( (nearGridID < 0) || (nearGridID > ix*iy*iz-1) ) continue;
// 				int32_t staJID = nearGridID == 0 ? 0 : dData.numPartInGrids[nearGridID-1];
// 				int32_t endJID = dData.numPartInGrids[nearGridID];
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
									DeviceData &dData,
									computeConstraintKernelVariables &KV)
{
	int32_t IID = KV.IID;
	glm::vec3 displaceVectorIJ = dData.correctedPos[IID] - dData.correctedPos[JID];
	float distanceIJ = sqrt(glm::dot(displaceVectorIJ, displaceVectorIJ));

	if (distanceIJ < (KV.H * 0.5f))
	{
		float particleVolume = pow(2.0f * dData.commonParam->radius, 3);
		float kernelWeight = Poly6Kernel(0.5f * KV.H, distanceIJ);
		float densityJ0 = dData.phaseParam[dData.phases[JID]].density;

		KV.densityI += densityJ0 * particleVolume * kernelWeight;

		if (IID == JID) return;
		if (distanceIJ < KV.H * 0.00001f) return;

		glm::vec3 gradKernelWeight = SpikyGradKernel(0.5f * KV.H, displaceVectorIJ);
		glm::vec3 gradConstraintIJ = KV.iDensityI0 * densityJ0 * particleVolume * gradKernelWeight;
		KV.gradConstraintI += gradConstraintIJ;
		KV.gradConstraintSqrSum += dot(-gradConstraintIJ, -gradConstraintIJ);
	}
}

inline __device__ void ComputeConstraintToGlobal(DeviceData &dData,
										computeConstraintKernelVariables &KV)
{
	KV.gradConstraintSqrSum += dot(KV.gradConstraintI,KV.gradConstraintI);
	KV.constraintI = KV.densityI*KV.iDensityI0 - 1.0f;
	dData.lambdas[KV.IID] = - KV.constraintI / (KV.gradConstraintSqrSum + dData.commonParam->relaxationParameter);
}

__global__ void keComputeConstraint(DeviceData dData,
									glm::vec3 	v3MinPosition, 
									glm::vec3 	v3MaxPosition,
									int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		// KernelVariables KV; 
		// for_NearParticles(ComputeConstraint, dData, KV); ==> 안에서 IID는 고정, JID는 내부에서 계산
		// dData.update;

		computeConstraintKernelVariables KV; 
		KV.IID					= idx;
		KV.densityI0			= dData.phaseParam[dData.phases[KV.IID]].density;
		KV.iDensityI0			= 1.0f/dData.phaseParam[dData.phases[KV.IID]].density;
		KV.H					= dData.commonParam->radius * 1.2f * 2.0f * 2.0f;

		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dData.commonParam->radius) - v3MinPosition.x)/KV.H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dData.commonParam->radius) - v3MinPosition.y)/KV.H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dData.commonParam->radius) - v3MinPosition.z)/KV.H)+1;
		for (int32_t yyy = -1 ; yyy < 2  ; ++yyy)
			for (int32_t zzz = -1 ; zzz < 2  ; ++zzz)
				for (int32_t xxx = -1 ; xxx < 2  ; ++xxx)
				{
					int32_t nearGridID = dData.gridIndices[idx] + xxx + ix*zzz + ix*iz*yyy;
					if ( (nearGridID < 0) || (nearGridID > ix*iy*iz-1) ) continue;
					int32_t staJID = nearGridID == 0 ? 0 : dData.numPartInGrids[nearGridID-1];
					int32_t endJID = dData.numPartInGrids[nearGridID];
					for (int32_t JID = staJID; JID < endJID; ++JID)
					{
						// 모두 이런 형식일 것이므로!
						ComputeConstraint(JID, dData, KV);
					}
				}
		ComputeConstraintToGlobal(dData, KV);
	}
}

__global__ void keComputePositionCorrection(DeviceData dData,
											glm::vec3 	v3MinPosition, 
											glm::vec3 	v3MaxPosition,
											int64_t 	nParticles)
{
	int32_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	int32_t IID = idx;

	if(idx < nParticles)
	{
		float density0 = dData.phaseParam[dData.phases[IID]].density;
		float iDensity0 = 1.0f/density0;
		dData.deltaPos[IID] = glm::vec3(0.0f);
		float H = dData.commonParam->radius * 1.2f * 2.0f * 2.0f;
		int32_t ix = static_cast<int32_t>((v3MaxPosition.x - (dData.commonParam->radius) - v3MinPosition.x)/H)+1;
		int32_t iy = static_cast<int32_t>((v3MaxPosition.y - (dData.commonParam->radius) - v3MinPosition.y)/H)+1;
		int32_t iz = static_cast<int32_t>((v3MaxPosition.z - (dData.commonParam->radius) - v3MinPosition.z)/H)+1;
		
		for (int32_t yyy = -1 ; yyy < 2  ; ++yyy)
			for (int32_t zzz = -1 ; zzz < 2  ; ++zzz)
				for (int32_t xxx = -1 ; xxx < 2  ; ++xxx)
				{
					int32_t nearGridID = dData.gridIndices[idx] + xxx + ix*zzz + ix*iz*yyy;
					if ( (nearGridID < 0) || (nearGridID > ix*iy*iz-1) ) continue;
					int32_t staJID = nearGridID == 0 ? 0 : dData.numPartInGrids[nearGridID-1];
					int32_t endJID = dData.numPartInGrids[nearGridID];
					for (int32_t JID = staJID; JID < endJID; ++JID)
					{
						glm::vec3 dr = dData.correctedPos[IID] - dData.correctedPos[JID];
						float dr2  = glm::dot(dr,dr);
						if ( dr2 < (H*H*0.25f) )
						{
							if (IID == JID) continue;
							if ( dr2 < (H*H*0.0000001f) ) continue;

							float volume = pow(2.0f*dData.commonParam->radius,3);
							float dlen   = sqrt(dr2);
							glm::vec3 gradKernel = SpikyGradKernel(0.5f*H, dr);
							
							float scorr = - dData.commonParam->scorrK * powf(Poly6Kernel(0.5f*H, dlen) / Poly6Kernel(0.5f*H, 0.5f*H*dData.commonParam->scorrDq),4.0f);

							dData.deltaPos[IID] += iDensity0 * ((dData.lambdas[IID] + dData.lambdas[JID])*0.5f + scorr) * dData.phaseParam[dData.phases[JID]].density * volume * gradKernel;							
						}
					}
				}
		// bool check = false;
		// if (dData.correctedPos[IID].x < dData.commonParam->AnalysisBox.minPoint.x)  check = true;
		// if (dData.correctedPos[IID].x > dData.commonParam->AnalysisBox.maxPoint.x)  check = true;
		// if (dData.correctedPos[IID].y < dData.commonParam->AnalysisBox.minPoint.y)  check = true;
		// if (dData.correctedPos[IID].y > dData.commonParam->AnalysisBox.maxPoint.y)  check = true;
		// if (dData.correctedPos[IID].z < dData.commonParam->AnalysisBox.minPoint.z)  check = true;
		// if (dData.correctedPos[IID].z > dData.commonParam->AnalysisBox.maxPoint.z)  check = true;
		// dData.deltaPos[IID] = -0.01f*dData.deltaPos[IID];
	}
}

__global__ void kePredictPosition(DeviceData dData, 
						 		int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		dData.velocities[idx] += dData.commonParam->dt * dData.commonParam->gravity;
		dData.correctedPos[idx] = dData.positions[idx] + dData.commonParam->dt*dData.velocities[idx];
	}
}


__global__ void keUpdateCorretedPosition(DeviceData dData, 
						 				int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		dData.correctedPos[idx] = dData.correctedPos[idx] + dData.deltaPos[idx];
		
		if (dData.correctedPos[idx].x < dData.commonParam->AnalysisBox.minPoint.x) dData.correctedPos[idx].x = dData.commonParam->AnalysisBox.minPoint.x;
		if (dData.correctedPos[idx].x > dData.commonParam->AnalysisBox.maxPoint.x) dData.correctedPos[idx].x = dData.commonParam->AnalysisBox.maxPoint.x;
		if (dData.correctedPos[idx].y < dData.commonParam->AnalysisBox.minPoint.y) dData.correctedPos[idx].y = dData.commonParam->AnalysisBox.minPoint.y;
		if (dData.correctedPos[idx].y > dData.commonParam->AnalysisBox.maxPoint.y) dData.correctedPos[idx].y = dData.commonParam->AnalysisBox.maxPoint.y;
		if (dData.correctedPos[idx].z < dData.commonParam->AnalysisBox.minPoint.z) dData.correctedPos[idx].z = dData.commonParam->AnalysisBox.minPoint.z;
		if (dData.correctedPos[idx].z > dData.commonParam->AnalysisBox.maxPoint.z) dData.correctedPos[idx].z = dData.commonParam->AnalysisBox.maxPoint.z;
	}
}

__global__ void keUpdateVelPos(DeviceData dData, 
						 		int64_t 	nParticles)
{
	int64_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < nParticles)
	{
		dData.velocities[idx] = (dData.correctedPos[idx] - dData.positions[idx])/dData.commonParam->dt;
		dData.positions[idx]  =  dData.correctedPos[idx];
		
		if (dData.positions[idx].x < dData.commonParam->AnalysisBox.minPoint.x) dData.positions[idx].x = dData.commonParam->AnalysisBox.minPoint.x;
		if (dData.positions[idx].x > dData.commonParam->AnalysisBox.maxPoint.x) dData.positions[idx].x = dData.commonParam->AnalysisBox.maxPoint.x;
		if (dData.positions[idx].y < dData.commonParam->AnalysisBox.minPoint.y) dData.positions[idx].y = dData.commonParam->AnalysisBox.minPoint.y;
		if (dData.positions[idx].y > dData.commonParam->AnalysisBox.maxPoint.y) dData.positions[idx].y = dData.commonParam->AnalysisBox.maxPoint.y;
		if (dData.positions[idx].z < dData.commonParam->AnalysisBox.minPoint.z) dData.positions[idx].z = dData.commonParam->AnalysisBox.minPoint.z;
		if (dData.positions[idx].z > dData.commonParam->AnalysisBox.maxPoint.z) dData.positions[idx].z = dData.commonParam->AnalysisBox.maxPoint.z;
	}
}