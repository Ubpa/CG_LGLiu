#ifndef POLYGON_H
#define POLYGON_H

#include "Figure.h"
#include <vector>

class Polygon : public Figure{
public:
    Polygon(const QPoint _start_point);

    // 用于动态更新
	void update(const QPoint _end_point);

    // 用于改变 Polygon 的状态
    // mode == 0 ：绘制完毕
    // mode == 1 : 新增一个点
    void update(int mode);
    void Draw(QPainter &paint);

private:
    // QPolygon 继承于 QVector, 类似于 vector
    // 能直接用于接口 drawPolygon 和 drawPolyline
    QPolygon points;
    bool done;
};

#endif // POLYGON_H