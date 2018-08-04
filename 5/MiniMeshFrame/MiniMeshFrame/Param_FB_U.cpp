#include "Param_FB_U.h"

std::vector<float> Param_FB_U::getWeights(HE_vert * vert, Mesh3D * ptr_mesh){
	float weight = 1.0f / vert->neighborIdx.size();
	std::vector<float> weights(vert->neighborIdx.size(), weight);
	return weights;
}
