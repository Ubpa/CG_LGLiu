#include "Quat.h"

Quat::Quat()
:x(0.0L), y(0.0L), z(0.0L), w(0.0L){
}

Quat::Quat(double _x, double _y, double _z, double _w)
:x(_x), y(_y), z(_z), w(_w)
{
}

Quat::Quat(Vector3d norm, double theta){
	double a = sin(theta/2.0L);
	x = a * norm.x();
	y = a * norm.y();
	z = a * norm.z();
	w = cos(theta/2.0L);
}

Quat::~Quat()
{
}

Quat Quat::operator ()(double t){
	// t : 0 - 1
	if(t<0.0L || t > 1.0L)
		return InValid();
	
	double theta_over_2 = acos(w);
	double a = sin(theta_over_2);
	double nx = ISZERO(a)?0.0L:(x/a);
	double ny = ISZERO(a)?0.0L:(y/a);
	double nz = ISZERO(a)?0.0L:(z/a);
	
	double new_theta_over_2 = theta_over_2 * t;
	double newA = sin(new_theta_over_2);
	
	return Quat(nx * newA, ny * newA, nz * newA, cos(new_theta_over_2));
}

Quat & Quat::operator =(Quat & q){
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	return (*this);
}