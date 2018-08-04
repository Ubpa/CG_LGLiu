#include "Polygon.h"

Polygon::Polygon(const QPoint _start_point){
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
    if(done)
		paint.drawPolygon(points);
    else
        paint.drawPolyline(points);
}

void Polygon::update(const QPoint _end_point){
    if(points.size()>0)
		points.back() = _end_point;
}