#include "Interpolation.h"

Interpolation::~Interpolation(){
}

ND_Point Interpolation::map(ND_Point & p){
	return p;
}

QImage * Interpolation::warp(QImage * img, std::vector<ND_Point> & points, MatrixXi * flag){
	START();

	// warp all point in [ points ]
	QImage * newImg = new QImage(img->width(), img->height(), QImage::Format_RGB888);
	for(int i=0; i < points.size(); i++){
		ND_Point p = points[i];
		// map is a virtual function
		points[i] = map(p);
		ND_Point q = p + points[i];
		if(img->rect().contains(q)){
			if(flag != NULL)
				(*flag)(q.x, q.y) = 1;
			if(img->rect().contains(p) && newImg->rect().contains(q))
				newImg->setPixel(q, img->pixel(p));
		}
	}

	END("Time of idw/rbf warping");

	return newImg;
}