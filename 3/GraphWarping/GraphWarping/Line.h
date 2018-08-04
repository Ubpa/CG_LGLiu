#ifndef LINE_H
#define LINE_H

#include <qpainter.h>
#include <qpoint.h>
#include <qpen.h>

class Line{
public:
    Line(const QPoint _start_point, const QPoint _end_point);
    void Draw(QPainter &paint);

    // 用于动态更新
    void update(const QPoint _end_point);
	QPoint & getStartPoint();
	QPoint & getEndPoint();

private:
    // 一开始想用自己定义的 struct point，但发现 QT 有 QPoint
    // 同样的之后使用了 QPolygon， QPainterPath 等
    QPoint start_point, end_point;
	QPen linePen, pointPen;
};

#endif //LINE_H