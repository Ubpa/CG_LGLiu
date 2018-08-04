#include "Param_FB_WLS.h"

std::vector<float> Param_FB_WLS::getWeights(HE_vert * vert, Mesh3D * ptr_mesh){
	std::vector<float> one_over_d, weights;

	float sum = 0;
	for(int i=0; i<vert->neighborIdx.size(); i++){
		one_over_d.push_back(1.0f / (vert->position() - ptr_mesh->get_vertex(vert->neighborIdx[i])->position()).length());
		sum += one_over_d.back();
	}

	for(int i=0; i<vert->neighborIdx.size(); i++)
		weights.push_back(one_over_d[i] / sum);

	return weights;
}