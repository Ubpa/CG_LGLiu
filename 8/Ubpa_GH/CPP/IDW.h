#ifndef IDW_H
#define IDW_H

#include "Interpolation.h"
#include <cmath>

class IDW : public Interpolation{
public:
	IDW(std::vector<ND_Point> & _pVec, std::vector<ND_Point> & _fpVec, float u=2);
	~IDW();

	ND_Point map(ND_Point & p);

private:
	std::vector<ND_Point> pVec;
	std::vector<ND_Point> fpVec;
	float u;
};

#endif // IDW_H