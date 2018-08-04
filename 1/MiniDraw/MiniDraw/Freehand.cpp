#include "Freehand.h"

Freehand::Freehand(const QPoint _start_point){
    path.moveTo(_start_point);
}

void Freehand::update(const QPoint _end_point){
    if(path.elementCount() == 0)
        path.moveTo(_end_point);
	else
        path.lineTo(_end_point);
}

void Freehand::Draw(QPainter &paint){
    paint.drawPath(path);
}