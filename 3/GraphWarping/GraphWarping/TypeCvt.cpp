#include "TypeCvt.h"

cv::Mat TypeCvt::QImageToMat(QImage & qImg){
	cv::Mat mat(cv::Size(qImg.width(), qImg.height()), CV_8UC3);
	for(int i=0; i<qImg.height(); i++){
		for(int j=0; j<qImg.width(); j++){
			cv::Vec3b & item = mat.at<cv::Vec3b>(i, j);
			QColor color = QColor(qImg.pixel(j, i));
			item[0] = color.red();
			item[1] = color.green();
			item[2] = color.blue();
		}
	}
	return mat;
}

QImage TypeCvt::MatToQImage(cv::Mat & mat){
	QImage qImg(mat.cols, mat.rows, QImage::Format_RGB888);
	for(int i=0; i<qImg.width(); i++){
		for(int j=0; j<qImg.height(); j++){
			cv::Vec3b & item = mat.at<cv::Vec3b>(j, i);
			qImg.setPixel(QPoint(i,j), QColor(item[0], item[1], item[2]).rgb());
		}
	}
	return qImg;
}