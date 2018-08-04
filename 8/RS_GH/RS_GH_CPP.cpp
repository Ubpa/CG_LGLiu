#include "RS_GH_CPP.h"

bool RS_CPP(Input_CPP * input_cpp, Output_CPP * output_cpp){
	output_cpp->r_array = new int[input_cpp->n];
	output_cpp->n = input_cpp->n;
	for(int i=0; i < input_cpp->n; i++){
		output_cpp->r_array[i] = input_cpp->_array[i];
		output_cpp->sum += input_cpp->_array[i];
	}
	return true;
}
