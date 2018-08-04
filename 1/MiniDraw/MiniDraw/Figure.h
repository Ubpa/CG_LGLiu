#ifndef FIGURE_H
#define FIGURE_H

#include <qpainter.h>
#include <qpoint.h>

class Figure{
public:
    virtual ~Figure(){};
    virtual void Draw(QPainter &paint)=0;

    // 提供动态更新的接口
    virtual void update(const QPoint _end_point){};

    // 提供给 Polygon 的接口
    virtual void update(int mode){};
};

#endif //FIGURE_H