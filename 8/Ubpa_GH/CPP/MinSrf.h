#ifndef MINSRF_H
#define MINSRF_H

#include "HE_mesh/Mesh3D.h"
#include "ND_Point.h"
#include "Util.h"
#include <vector>
#include <cmath>
#include "HE_mesh\Mesh3D.h"
#include "triangle.h"
#include <Eigen\Dense>
#include <Eigen\Sparse>
#define SMALLVALUE  10E-6
#define PI 3.1415926
using namespace Eigen;
typedef SparseMatrix<float> SpMat; // 声明一个列优先的双精度稀疏矩阵类型
typedef Triplet<float> T; //三元组（行，列，值）

class MinSrf{
public:
	virtual void Minimize(Mesh3D * mesh)=0;
};

class MinSrfLocal : public MinSrf
{
public:
	void Minimize(Mesh3D * mesh);
};

class MinSrfGlobal : public MinSrf
{
public:
	void Minimize(Mesh3D * mesh);
};

class MinSrfCurve : public MinSrfGlobal
{
public:
	void Minimize(std::vector<point> & curve, Mesh3D * mesh);
};

#endif// MINSRF_H