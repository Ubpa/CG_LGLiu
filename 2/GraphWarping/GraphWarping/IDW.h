#ifndef IDW_H
#define IDW_H

#include "Interpolation.h"
#include <qcolor.h>
#include <cmath>


class IDW : public Interpolation{
public:
	IDW(std::vector<QPoint *> *_pVec, std::vector<QPoint *> *_qVec);
	IDW(std::vector<QPoint *> *_pVec, std::vector<QColor *> *_colorVec);
	~IDW();

	QPoint map(QPoint p);
	// 颜色插值，用于填补空白区域
	QColor mapColor(QPoint p);

private:
	// 计算相似度
	double calSim(std::vector<double> * simVec, QPoint p);
	
	std::vector<QPoint *> pVec;
	std::vector<QPoint *> qVec;
	std::vector<QColor *> colorVec;

	double u;
};

#endif // IDW_H