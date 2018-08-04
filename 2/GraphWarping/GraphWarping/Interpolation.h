#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <qpoint.h>
#include <vector>
#include <QPointF>

class Interpolation{
public:
	Interpolation();
	~Interpolation();

	virtual QPoint map(QPoint p);

private:
	std::vector<QPoint *> pVec, qVec;
};

#endif // INTERPOLATION+H