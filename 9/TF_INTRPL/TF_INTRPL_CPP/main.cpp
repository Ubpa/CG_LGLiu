#include "API.h"
#include <cstdio>
#include <cmath>
#include <iostream>

using namespace std;

int main(){
	std::vector<Vector4d> p, Ap;
	p.push_back(Vector4d(0,0,0,1));
	p.push_back(Vector4d(0,0,5,1));
	p.push_back(Vector4d(0,5,0,1));
	p.push_back(Vector4d(0,2.5,5,1));
	Ap.push_back(Vector4d(0,100,0,1));
	Ap.push_back(Vector4d(0,100,5,1));
	Ap.push_back(Vector4d(5,100,0,1));
	Ap.push_back(Vector4d(2.5,100,5,1));
	/*
	Transform tf = Transform(p, fp);
	for(int i=0; i<p.size(); i++){
		cout << (tf(1.0L)*p[i]).transpose() << endl;
	}*/
	API * api = new API();
	api->num = p.size();
	api->px = new double[api->num];
	api->py = new double[api->num];
	api->pz = new double[api->num];
	api->Apx = new double[api->num];
	api->Apy = new double[api->num];
	api->Apz = new double[api->num];
	api->transform = new double[16];
	for(int i=0; i<api->num; i++){
		api->px[i] = p[i].x();
		api->py[i] = p[i].y();
		api->pz[i] = p[i].z();
		api->Apx[i] = Ap[i].x();
		api->Apy[i] = Ap[i].y();
		api->Apz[i] = Ap[i].z();
	}
	
	//api->t = 0.0L;
	//api->t = 0.5L;
	api->t = 0.7L;

	if(!TF_Interpolation_CPP(api)){
		cout<<"error"<<endl;
		return 0;
	}

	for(int i=0; i<api->num; i++){
		printf("%f, %f, %f\n", api->Apx[i], api->Apy[i], api->Apz[i]);
	}
	printf("\nT:\n");
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			printf("%f ", api->transform[i*4+j]);
		}
		printf("\n");
	}

	return 0;
}
