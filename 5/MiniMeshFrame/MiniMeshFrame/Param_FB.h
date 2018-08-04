#pragma once
#include "Param.h"
class Param_FB :
	public Param
{
public:
	void Parameterize(Mesh3D * ptr_mesh);
private:
	void getBoundary(Mesh3D * ptr_mesh);
	void setBoundary(Mesh3D * ptr_mesh);
	void mapToRegion2D(Mesh3D * ptr_mesh);
	virtual std::vector<float> getWeights(HE_vert * vert, Mesh3D * ptr_mesh) = 0;

	std::vector<int> boundary;
};
