#ifndef MESH_H
#define MESH_H

#include "Interpolation.h"
#include "Util.h"

//定义用于边表ET和活动边表AET的通用类Edge
typedef struct edge{
	int ymax;
	float x;
	float dx;
	struct edge * next;
} Edge;

typedef std::vector<int> Face;

class Mesh : public Interpolation{
public:
	// generate mesh
	void loadRectMesh(Img * img, int n);
	void loadTriRandMesh(Img * img, int n);
	void loadTriFeatureMesh(Img * img);

	std::vector<ND_Point> face2PVec(Face & face);

	Img * warp(Img * img, Interpolation & interpolation);
	std::vector<ND_Point> polygonScan(std::vector<ND_Point> & polygon, int windowHeight);

private:
	//简单的使用 contains 来判断点是否在polygon内
	//std::vector<ND_Point> getPInPolygon(std::vector<ND_Point> & polygon);
	ND_Point getOriginalP(ND_Point & p, Face & face);
	ND_Point getOriginalP_Rect(ND_Point & p, Face & face);
	ND_Point getOriginalP_Tri(ND_Point & p, Face & face);

	std::vector<ND_Point> getRandP(int width, int height, int n);

	std::vector<ND_Point> V, oldV;
	std::vector<Face> faces;
};

#endif // MESH_H