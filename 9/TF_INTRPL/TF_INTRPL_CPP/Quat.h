#pragma once

#include <vector>
#include <cmath>
#include <Eigen\Dense>

typedef Eigen::MatrixXd MatrixXd;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::Vector4d Vector4d;
typedef Eigen::Matrix<double,4,4> TF_M;

#define PI 3.1415926
#define SMALLVALUE 10E-6
#define ABS(x) ((x)<0.0L?-(x):(x))
#define ISZERO(x) (ABS((x))<SMALLVALUE?true:false)
#define SIGN(x) ((x)<0.0L?-1.0L:1.0L)
#define SQRT(x) ((x)<0.0L?0.0L:sqrt(x))

class Quat
{
public:
	double x, y, z, w;

	Quat();
	Quat(double _x, double _y, double _z, double _w);
	Quat(Vector3d norm, double theta);
	~Quat();

	inline Vector3d norm(){
		double a = sin(acos(w));
		return Vector3d(x/a, y/a, z/a);
	}

	inline static Quat InValid(){ return Quat(0.0L, 0.0L, 0.0L, 0.0L); };
	inline double theta(){ return 2.0L * acos(w); }

	Quat operator ()(double t);
	Quat & operator =(Quat & q);
};

