#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <vector>
#include <qimage.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "triangle.h"
#include "TypeCvt.h"
#include "ND_Point.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <Eigen/Dense>
using namespace Eigen;

#define START() clock_t start_time = clock()
#define END(str) \
clock_t end_time = clock();\
std::cout<< (str) << " : "\
	<<static_cast<float>(end_time-start_time)/CLOCKS_PER_SEC*1000\
	<<"ms"<<std::endl;//输出运行时间

class Interpolation{
public:
	virtual ~Interpolation();

	virtual ND_Point map(ND_Point & p);
	virtual QImage * warp(QImage * img, std::vector<ND_Point> & points, MatrixXi * flag = NULL);
};

#endif // INTERPOLATION_H