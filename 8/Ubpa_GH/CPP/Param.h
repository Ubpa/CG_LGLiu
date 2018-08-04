#pragma once
#include <vector>
#include <cmath>
#include "HE_mesh\Mesh3D.h"
#include <Eigen\Dense>
#include <Eigen\Sparse>
#define SMALLVALUE  10E-6
#define PI 3.1415926
using namespace Eigen;
typedef SparseMatrix<float> SpMat; // 声明一个列优先的双精度稀疏矩阵类型
typedef Triplet<float> T; //三元组（行，列，值）

class Param
{
public:
	virtual ~Param(){};
	virtual void Parameterize(Mesh3D * ptr_mesh) = 0;
};

