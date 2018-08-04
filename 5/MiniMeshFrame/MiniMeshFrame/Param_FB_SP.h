#pragma once
#include "param_fb.h"

class Param_FB_SP :
	public Param_FB
{
private:
	std::vector<float> getWeights(HE_vert * vert, Mesh3D * ptr_mesh);
};

