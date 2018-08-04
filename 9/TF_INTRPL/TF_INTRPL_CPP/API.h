#ifndef API_H
#define API_H

#include "Transform.h"

struct API{
	double * px;
	double * py;
	double * pz;

	double * Apx;
	double * Apy;
	double * Apz;
	// num >= 3
	int num;
	double t;
	// 4 x 4
	double * transform;
};

extern "C" __declspec(dllexport) bool TF_Interpolation_CPP(API * api);

#endif API_H