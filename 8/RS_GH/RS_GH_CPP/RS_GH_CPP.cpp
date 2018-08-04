#include "RS_GH_CPP.h"

bool RS_CPP(API_CPP * api_cpp_in, API_CPP * api_cpp_out){
	api_cpp_out->_array = new int[api_cpp_in->n];
	api_cpp_out->n = api_cpp_in->n;
	for(int i=0; i < api_cpp_in->n; i++){
		api_cpp_out->_array[i] = api_cpp_in->_array[i];
	}
	return true;
}
