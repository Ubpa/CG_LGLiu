#ifndef RBF_H
#define RBF_H

#include "interpolation.h"
#include <cmath>

class RBF : public Interpolation{
public:
	RBF(std::vector<ND_Point> & _pVec, std::vector<ND_Point> &_fpVec, float _r = 25, float _u = 1);
	~RBF();

	ND_Point map(ND_Point & p);

private:
	std::vector<ND_Point> pVec, fpVec;

	// RBF相关的所有系数, RBF_A = [a, alpha]
	MatrixXd *RBF_A;
	float r, u;
};

#endif // RBF_H

