class Input_CPP{
public:
	int * _array;
	int n;
};

class Output_CPP{
public:
	int * r_array;
	int n;
	int sum;
};

extern "C" __declspec(dllexport) bool RS_CPP(Input_CPP * input_cpp, Output_CPP * output_cpp);