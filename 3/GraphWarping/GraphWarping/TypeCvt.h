#ifndef TYPECVT_H
#define TYPECVT_H

#include "Interpolation.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

// ÀàÐÍ×ª»»
class TypeCvt{
public:
	static cv::Mat QImageToMat(QImage & qImg);
	static QImage MatToQImage(cv::Mat & mat);
};

#endif TYPECVT_H