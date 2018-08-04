#ifndef RECT_H
#define RECT_H

#include "Figure.h"

class Rect : public Figure{
public:
    Rect(const QPoint _start_point, const QPoint _end_point);
    void Draw(QPainter &paint);
    
    // 用于动态更新
    void update(const QPoint _end_point);

private:
	QPoint start_point, end_point;
};

#endif // RECT_H