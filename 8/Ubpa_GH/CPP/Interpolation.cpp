#include "Interpolation.h"

Interpolation::~Interpolation(){
}

ND_Point Interpolation::map(ND_Point & p){
	return p;
}

Img * Interpolation::warp(Img * img, std::vector<ND_Point> & points, MatrixXi * flag){
	// warp all point in [ points ]
	Img * newImg = new Img(img->width, img->height);
	for(int i=0; i < points.size(); i++){
		ND_Point p = points[i];
		// map is a virtual function
		points[i] = map(p);
		ND_Point q = p + points[i];
		if(img->contains(q)){
			if(flag != NULL)
				(*flag)(q.x, q.y) = 1;
			if(img->contains(p) && newImg->contains(q))
				newImg->at(q) = img->at(p);
		}
	}

	return newImg;
}
