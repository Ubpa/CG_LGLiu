#ifndef ND_POINT_H
#define ND_POINT_H

#include <qpoint.h>
#include <qcolor.h>
#include <vector>

struct ND_Point{
	int dim;
	int x;
	int y;
	int z;

	ND_Point(const ND_Point & p);
	ND_Point(std::vector<int> & _data);
	ND_Point(std::vector<float> & _data);
	ND_Point(QPoint p);
	ND_Point(QColor c);
	ND_Point(int _x, int _y);
	ND_Point(int _x, int _y, int _z);

	inline int mod2(){ return x*x + y*y + z*z; }
	inline float mod(){ return sqrt((float)mod2()); }

	ND_Point & operator =(ND_Point & p);
	ND_Point & operator +=(ND_Point & p);
	inline ND_Point operator +(ND_Point & p){ return ND_Point(x+p.x, y+p.y, z+p.z); }
	inline ND_Point operator -(ND_Point & p){ return ND_Point(x-p.x, y-p.y, z-p.z); }
	inline ND_Point operator *(float a){ return ND_Point(x*a, y*a, z*a); }
	inline ND_Point operator /(float a){ return ND_Point(x/a, y/a, z/a); }

	inline operator QPoint(){ return QPoint(x, y); }
	inline operator QColor(){ return QColor(x, y, z); }
	
	// 计算叉乘
	static inline int crossProduct(ND_Point & p1, ND_Point & p2){ return p1.x*p2.y-p2.x*p1.y; };
	// 计算三角形面积
	static inline float SofTri(ND_Point & p1, ND_Point & p2, ND_Point & p3){
		return ND_Point::crossProduct(p2 - p1, p3 - p1);
	}
};

#endif