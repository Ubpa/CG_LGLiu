#include <iostream>
#include <vector>
#include "IDW.h"
#include "RBF.h"
#include "Mesh.h"
#include "Util.h"
#include "MinSrf.h"
#include "Param_FB_SP.h"
#include "Param_FB_U.h"
#include "Param_FB_WLS.h"
#include "Param_UFB_ARAP.h"
#include <cmath>
#include <malloc.h>

#include <Eigen/Dense>
using namespace Eigen;

using namespace std;

struct WarpAPI{
	// image
	int * R;
	int * G;
	int * B;
	int width;
	int height;
	// input
	int * px;
	int * py;
	int * fpx;
	int * fpy;
	int n;
	// method
	void convertTo(vector<ND_Point> & p, vector<ND_Point> & fp, Img & img);
	void loadFrom(Img & img);
};

struct MeshAPI
{
	int VerticesNumber;
	int FaceNumber;
	int TexCoordNumber;

	float* VerticesX;
	float* VerticesY;
	float* VerticesZ;

	int* FaceA;
	int* FaceB;
	int* FaceC;

	float* TexCoordX;
	float* TexCoordY;
	// method
	void convertTo(Mesh3D & mesh);
	void updateVFrom(Mesh3D & mesh);
	void updateTFrom(Mesh3D & mesh);
	/*-- only load V and F --*/
	void loadFrom(Mesh3D & mesh);
};

extern "C" __declspec(dllexport) bool WarpIDW_CPP(WarpAPI * api);
extern "C" __declspec(dllexport) bool WarpRBF_CPP(WarpAPI * api);
extern "C" __declspec(dllexport) bool MeshRect_CPP(WarpAPI * api);
extern "C" __declspec(dllexport) bool MeshTriR_CPP(WarpAPI * api);
extern "C" __declspec(dllexport) bool MeshTriF_CPP(WarpAPI * api);

extern "C" __declspec(dllexport) bool MinSrfLocal_CPP(MeshAPI * api);
extern "C" __declspec(dllexport) bool MinSrfGlobal_CPP(MeshAPI * api);
extern "C" __declspec(dllexport) bool MinSrfCurve_CPP(MeshAPI * api_in, MeshAPI * api_out);

extern "C" __declspec(dllexport) bool Param_FB_U_CPP(MeshAPI * api);
extern "C" __declspec(dllexport) bool Param_FB_WLS_CPP(MeshAPI * api);
extern "C" __declspec(dllexport) bool Param_FB_SP_CPP(MeshAPI * api);
extern "C" __declspec(dllexport) bool Param_UFB_ARAP_CPP(MeshAPI * api);

extern "C" __declspec(dllexport) bool MeshAPIFree_CPP(MeshAPI * api);