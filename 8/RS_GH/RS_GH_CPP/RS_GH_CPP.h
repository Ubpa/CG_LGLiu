
#include <cstdlib>

struct API_CPP{
	int * _array;
	int n;
};

extern "C" __declspec(dllexport) bool RS_CPP(API_CPP * api_in_cpp, API_CPP * api_out_cpp);