#include "Line.h"

Line::Line(const QPoint _start_point, const QPoint _end_point){
	start_point.setX(_start_point.x());
    start_point.setY(_start_point.y());
	end_point.setX(_end_point.x());
    end_point.setY(_end_point.y());

	linePen.setWidth(3);
	linePen.setColor(QColor(255, 0, 0));

	pointPen.setWidth(5);
	pointPen.setColor(QColor(255, 242, 0));
}

void Line::Draw(QPainter &paint){
	// back-up
	const QPen & oldPen = paint.pen();

	// draw line
    paint.setPen(linePen);
	paint.drawLine(start_point, end_point);

	// draw two endpoints of line
	paint.setPen(pointPen);
	paint.drawPoint(start_point);
	paint.drawPoint(end_point);

	// restore
	paint.setPen(oldPen);
};

void Line::update(const QPoint _end_point){
	end_point.setX(_end_point.x());
    end_point.setY(_end_point.y());
}

QPoint & Line::getStartPoint(){
	return start_point;
}

QPoint & Line::getEndPoint(){
	return end_point;
}