#include "ND_Point.h"

ND_Point::ND_Point(const ND_Point & p)
:dim(p.dim), x(p.x), y(p.y), z(p.z){
}

ND_Point::ND_Point(std::vector<int> & _data)
:dim(_data.size()), x(_data[0]), y(_data[1]){
	z = dim == 3 ? _data[2] : 0;
}

ND_Point::ND_Point(std::vector<float> & _data)
:dim(_data.size()), x(_data[0]), y(_data[1]){
	z = dim == 3 ? _data[2] : 0;
}

ND_Point::ND_Point(QPoint p)
:dim(2), x(p.x()), y(p.y()), z(0){
}

ND_Point::ND_Point(QColor c)
:dim(3), x(c.red()), y(c.green()), z(c.blue()){
}

ND_Point::ND_Point(int _x, int _y)
:dim(2), x(_x), y(_y), z(0){
}

ND_Point::ND_Point(int _x, int _y, int _z)
:dim(3), x(_x), y(_y), z(_z){
}

ND_Point & ND_Point::operator =(ND_Point & p){
	dim = p.dim;
	x = p.x;
	y = p.y;
	z = p.z;
	return (*this);
}

ND_Point & ND_Point::operator +=(ND_Point & p){
	x += p.x;
	y += p.y;
	z += p.z;
	return (*this);
}