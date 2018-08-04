#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <qfiledialog.h>
#include <iostream>
#include <vector>
#include "Interpolation.h"
#include "IDW.h"
#include "RBF.h"
#include "Line.h"
#include <Eigen/Dense>
#include <cmath>
#include <qstatusbar.h>
using namespace Eigen;

enum MODE{
	IDLE,//等待
	GET,//获取数据点
	DRAW,//绘制线中
};

class ImageWidget : public QWidget{
	Q_OBJECT

public:
	ImageWidget();
	~ImageWidget();

	// event
	void mousePressEvent(QMouseEvent * mouseEvent);
	void mouseMoveEvent(QMouseEvent * mouseEvent);
	void mouseReleaseEvent(QMouseEvent * mouseEvent);
    void paintEvent(QPaintEvent *);

	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// warp
	void Get();
	void WarpIDW();
	void WarpRBF();
	// 填补空白
	void fix(QImage * newImage, MatrixXd *flag);

	// 在get时可撤销数据点，在非get时可撤销图像变形（idw，rbf）操作
	void Undo();
	

private:
	// 将lineVec的所有成员转化成QPoint并存入p和q中
	void lineVecTo(std::vector<QPoint *> *p, std::vector<QPoint *> *q);
	void warp(Interpolation * interpolation);
	// 用于判断鼠标的位置是否在图像内
	bool insideImg(QPoint p);

	QImage *currentImage;
	Line *currentLine;
	std::vector<Line *> lineVec;
	std::vector<QImage *> imageVec;
	QString filename;
	int mode;
};

#endif // IMAGEWIDGET_H
