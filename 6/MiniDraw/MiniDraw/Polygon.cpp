#include "Polygon.h"

Polygon::Polygon(const QPoint _start_point)
:Figure(){
    points.push_back(_start_point);
    done = false;
};

void Polygon::update(int mode){
    switch(mode){
    case 0:
        // 绘制完毕
        done = true;
        break;
    case 1:
        // 增加一个点
        points.push_back(points.back());
        break;
    default:
        break;
    }
}

void Polygon::Draw(QPainter &paint){
	QPen oldPen = paint.pen();

	// draw line
    if(linePen != nullptr)
		paint.setPen(*linePen);

    if(done)
		paint.drawPolygon(points);
    else
        paint.drawPolyline(points);

	// draw point
	if(canDrag){
		if(pointPen != nullptr)
			paint.setPen(*pointPen);
		for(int i=0; i<points.size(); i++)
			paint.drawPoint(points[i]);
	}
	
	// restore pen
	paint.setPen(oldPen);
}

void Polygon::update(const QPoint _end_point){
    if(points.size()>0)
		points.back() = _end_point;
}

std::vector<QPoint *> Polygon::getPoint(QPoint & pos, bool all){
	std::vector<QPoint *> rst;
	int i;
	for(i=0; i<points.size(); i++){	
		if(isClose(points[i], pos, 5)){
			rst.push_back(&points[i]);
			break;
		}
	}

	if(rst.size() == 1 && all){
		for(int j=0; j<points.size(); j++){
			if(j!=i)
				rst.push_back(&points[j]);
		}
	}
	return rst;
}