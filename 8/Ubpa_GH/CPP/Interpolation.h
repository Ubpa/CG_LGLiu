#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <vector>
#include "Img.h"
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "triangle.h"
#include "ND_Point.h"
#include <Eigen/Dense>
using namespace Eigen;

class Interpolation{
public:
	virtual ~Interpolation();

	virtual ND_Point map(ND_Point & p);
	virtual Img * warp(Img * img, std::vector<ND_Point> & points, MatrixXi * flag = NULL);
};

#endif // INTERPOLATION_H