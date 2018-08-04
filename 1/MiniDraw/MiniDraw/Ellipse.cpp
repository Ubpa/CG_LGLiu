#include "Ellipse.h"

Ellipse::Ellipse(const QPoint _start_point, const QPoint _end_point){
	start_point.setX(_start_point.x());
    start_point.setY(_start_point.y());
	end_point.setX(_end_point.x());
    end_point.setY(_end_point.y());
}

void Ellipse::update(const QPoint _end_point){
    end_point.setX(_end_point.x());
    end_point.setY(_end_point.y());
}

void Ellipse::Draw(QPainter &paint){
	paint.drawEllipse( start_point.x(), start_point.y(),
        end_point.x() - start_point.x(), end_point.y() - start_point.y());
}