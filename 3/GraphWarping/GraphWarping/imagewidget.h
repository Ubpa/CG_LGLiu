#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H
 
#include <QWidget>
#include <QImage>
#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QtWidgets\qtoolbar.h>
#include <qfiledialog.h>
#include <iostream>
#include <vector>
#include "Interpolation.h"
#include "IDW.h"
#include "RBF.h"
#include "Line.h"
#include "Mesh.h"
#include "TypeCvt.h"
#include <cmath>
#include <qstatusbar.h>
#include <malloc.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <Eigen/Dense>
using namespace Eigen;



#define FILETOOLNUM 3
#define WARPTOOLNUM 8
#define TOOLNUM ((FILETOOLNUM)+(WARPTOOLNUM))
#define PREFIX (std::string(":/GraphWarping/Resources/images/"))

// 采用事件机制来控制 GraphWarping 的 tool 的 icon
#define SW(tool, status) (parent()->event(&QEvent(eventType[((tool)<<1)|(status)])))
// status
#define INACTIVATE 0
#define ACTIVATE 1

namespace TOOL{
	enum {
		OPEN,
		SAVE,
		SAVEAS,
		GET,
		FEATURE,
		IDW,
		RBF,
		RECT_MESH,
		TRI_RAND_MESH,
		TRI_FEATURE_MESH,
		UNDO,
	};
}

enum MODE{
	IDLE,//等待
	GET,//获取数据点
	DRAW,//绘制线中
};

// FP 是 函数指针
class ImageWidget;//声明提前
typedef void (ImageWidget::* FP)();

class ImageWidget : public QWidget{
	Q_OBJECT

public:
	ImageWidget(QEvent::Type * _eventType);
	~ImageWidget();

	// event
	void mousePressEvent(QMouseEvent * mouseEvent);
	void mouseMoveEvent(QMouseEvent * mouseEvent);
	void mouseReleaseEvent(QMouseEvent * mouseEvent);
    void paintEvent(QPaintEvent *);

	// File IO
	void Open();
	void Save();
	void SaveAs();

	// warp
	void Get();
	void Feature();
	void WarpIDW();
	void WarpRBF();
	void WarpRectMesh();
	void WarpTriRandMesh();
	void WarpTriFeatureMesh();
	void Undo();// 在get时可撤销数据点，在非get时可撤销图像变形的操作

	// 填补空白
	static void fix(QImage * img, MatrixXi & flag);

	// 返回该类的函数指针
	static FP method(int id);

private:
	// 将lineVec的所有成员转化成QPoint并存入p和q中
	void lineVecTo(std::vector<ND_Point> & _pVec, std::vector<ND_Point> & fpVec);

	// 用于判断鼠标的位置是否在图像内
	std::vector<ND_Point> allImgP();

	// 获取图像的左上角的坐标，如果图像不存在，则返回(-1, -1)
	QPoint imgIdx();

	// 更新Icon
	void updateIcon();

	QImage * curImg;
	Line * curLine;
	std::vector<Line *> lineVec;
	std::vector<QImage *> imageVec;
	QString filename;
	int mode;
	
	// feature points of curImg
	std::vector<cv::KeyPoint> keypoints;
	//featureState==0 : 没有显示特征点
	//featureState==1 : 显示了特征点
	int featureState;

	// 自定义的 eventType
	// eventType[i] > QEvent::User, i = 0, 1, ..., 2*TOOLNUM-1
	QEvent::Type eventType[2 * TOOLNUM];
};

#endif // IMAGEWIDGET_H
