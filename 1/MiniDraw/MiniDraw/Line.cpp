#include "Line.h"

Line::Line(const QPoint _start_point, const QPoint _end_point){
	start_point.setX(_start_point.x());
    start_point.setY(_start_point.y());
	end_point.setX(_end_point.x());
    end_point.setY(_end_point.y());
}

void Line::Draw(QPainter &paint){
    paint.drawLine(start_point, end_point);
};

void Line::update(const QPoint _end_point){
	end_point.setX(_end_point.x());
    end_point.setY(_end_point.y());
}