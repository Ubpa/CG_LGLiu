#include "Param_FB_SP.h"

std::vector<float> Param_FB_SP::getWeights(HE_vert * vert, Mesh3D * ptr_mesh){
	std::vector<float> weights(vert->degree(), 0.0f);
	
	// compute all distance
	std::vector<float> d, c;
	for(int i=0; i<vert->degree(); i++){
		d.push_back((ptr_mesh->get_vertex(vert->neighborIdx[i])->position() - vert->position()).length());
		c.push_back((ptr_mesh->get_vertex(vert->neighborIdx[i])->position()
				- ptr_mesh->get_vertex(vert->neighborIdx[(i+1)%vert->degree()])->position()).length());
	}

	// compute angles
	std::vector<float> angles;
	float sumAngles = 0;
	for(int i=0; i<vert->degree(); i++){
		angles.push_back(acos((pow(d[i],2) + pow(d[(i+1)%vert->degree()],2) - pow(c[i],2)) / (2 * d[i] * d[(i+1)%vert->degree()])));
		sumAngles += angles.back();
	}
	
	// map neighbors to 2D region
	float theta = 0.0f;
	std::vector<point> neighbors;
	for(int i=0; i<vert->degree(); i++){
		neighbors.push_back(point(d[i] * cos(theta), d[i] * sin(theta), 0.0f));
		theta += 2 * PI * angles[i] / sumAngles;
	}
	
	// compute weights
	point p(0.0f, 0.0f, 0.0f);
	for(int i=0; i<vert->degree(); i++){
		point & p1 = neighbors[i];
		for(int j=0; j<vert->degree(); j++){
			int next = (j+1)%vert->degree();
			if(j!=i && next!=i){
				point & p2 = neighbors[j];
				point & p3 = neighbors[next];
				float S = trimesh::trinorm<point>(p1, p2, p3).length();
				float sigma1 = trimesh::trinorm<point>(p, p2, p3).length() / S;
				float sigma2 = trimesh::trinorm<point>(p1, p, p3).length() / S;
				float sigma3 = trimesh::trinorm<point>(p1, p2, p).length() / S;
				if(sigma1+sigma2+sigma3 < SMALLVALUE + 1){
					weights[i] += sigma1 / vert->degree();
					weights[j] += sigma2 / vert->degree();
					weights[next] += sigma3 / vert->degree();
					break;
				}
			}
		}
	}

	return weights;
}