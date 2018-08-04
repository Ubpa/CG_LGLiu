#include "API.h"

bool WarpIDW_CPP(WarpAPI * api){
	std::vector<ND_Point> p, fp;
	Img img;
	api->convertTo(p, fp, img);

	IDW idw(p, fp);

	MatrixXi flag(img.width, img.height);
	flag *= 0;
	
	Img * newImg = idw.warp(&img, allImgP(img), &flag);
	
	fix(newImg, flag);
	
	api->loadFrom(*newImg);

	delete newImg;
	
	return true;
}

bool WarpRBF_CPP(WarpAPI * api){
	std::vector<ND_Point> p, fp;
	Img img;
	api->convertTo(p, fp, img);

	RBF rbf(p, fp);

	MatrixXi flag(img.width, img.height);
	flag *= 0;
	
	Img * newImg = rbf.warp(&img, allImgP(img), &flag);
	
	fix(newImg, flag);
	
	api->loadFrom(*newImg);

	delete newImg;
	
	return true;
}

bool MeshRect_CPP(WarpAPI * api){
	std::vector<ND_Point> p, fp;
	Img img;
	api->convertTo(p, fp, img);

	IDW idw(p, fp);

	Mesh mesh;
	mesh.loadRectMesh(&img, 16);
	
	Img * newImg = mesh.warp(&img, idw);
	
	api->loadFrom(*newImg);

	delete newImg;
	
	return true;
}

bool MeshTriR_CPP(WarpAPI * api){
	std::vector<ND_Point> p, fp;
	Img img;
	api->convertTo(p, fp, img);

	IDW idw(p, fp);

	Mesh mesh;
	mesh.loadTriRandMesh(&img, 16);
	
	Img * newImg = mesh.warp(&img, idw);
	
	api->loadFrom(*newImg);

	delete newImg;
	
	return true;
}

bool MeshTriF_CPP(WarpAPI * api){
	std::vector<ND_Point> p, fp;
	Img img;
	api->convertTo(p, fp, img);

	IDW idw(p, fp);

	Mesh mesh;
	mesh.loadTriFeatureMesh(&img);
	
	Img * newImg = mesh.warp(&img, idw);
	
	api->loadFrom(*newImg);

	delete newImg;
	
	return true;
}
//------------------------------------------------------------------------------
bool MinSrfLocal_CPP(MeshAPI * api){
	Mesh3D mesh;

	api->convertTo(mesh);

	MinSrfLocal minSrf;
	minSrf.Minimize(&mesh);

	api->updateVFrom(mesh);

	mesh.ClearData();

	return true;
}

bool MinSrfGlobal_CPP(MeshAPI * api){
	Mesh3D mesh;

	api->convertTo(mesh);

	MinSrfGlobal minSrf;
	minSrf.Minimize(&mesh);

	api->updateVFrom(mesh);

	mesh.ClearData();

	return true;
}

bool MinSrfCurve_CPP(MeshAPI * api_in, MeshAPI * api_out){
	std::vector<point> curve;
	for(int i=0; i<api_in->VerticesNumber; i++){
		float x = api_in->VerticesX[i];
		float y = api_in->VerticesY[i];
		float z = api_in->VerticesZ[i];
		curve.push_back(point(x, y, z));
	}
	
	Mesh3D mesh;
	MinSrfCurve minSrf;
	minSrf.Minimize(curve, &mesh);

	api_out->loadFrom(mesh);

	mesh.ClearData();

	return true;
}
//------------------------------------------------------------------------------
void WarpAPI::convertTo(vector<ND_Point> & p, vector<ND_Point> & fp, Img & img){
	for(int i=0; i<n; i++){
		p.push_back(ND_Point(px[i], py[i]));
		fp.push_back(ND_Point(fpx[i], fpy[i]));
	}

	img.init(width, height);
	for(int i=0; i<width; i++){
		for(int j=0; j<height; j++)
			img.at(i,j) = ND_Point(R[i*width+j], G[i*width+j], B[i*width+j]);
	}
}

void WarpAPI::loadFrom(Img & img){
	width = img.width;
	height = img.height;
	/*
	R = new int[width*height];
	G = new int[width*height];
	B = new int[width*height];
	*/
	for(int i=0; i<width; i++){
		for(int j=0; j<height; j++){
			ND_Point color = img.at(i, j);
			R[i*width+j] = color.x;
			G[i*width+j] = color.y;
			B[i*width+j] = color.z;
		}
	}
}
//------------------------------------------------------------------------------
bool Param_FB_U_CPP(MeshAPI * api){
	Param_FB_U param;
	Mesh3D mesh;
	api->convertTo(mesh);
	param.Parameterize(&mesh);
	api->updateTFrom(mesh);
	return true;
}

bool Param_FB_WLS_CPP(MeshAPI * api){
	Param_FB_WLS param;
	Mesh3D mesh;
	api->convertTo(mesh);
	param.Parameterize(&mesh);
	api->updateTFrom(mesh);
	return true;
}
bool Param_FB_SP_CPP(MeshAPI * api){
	Param_FB_SP param;
	Mesh3D mesh;
	api->convertTo(mesh);
	param.Parameterize(&mesh);
	api->updateTFrom(mesh);
	return true;
}
bool Param_UFB_ARAP_CPP(MeshAPI * api){
	Param_FB_SP paramSP;
	Param_UFB_ARAP paramARAP;
	Mesh3D mesh;
	api->convertTo(mesh);
	paramSP.Parameterize(&mesh);
	paramARAP.Parameterize(&mesh);
	api->updateTFrom(mesh);
	return true;
}
//------------------------------------------------------------------------------
void MeshAPI::convertTo(Mesh3D & mesh){
	mesh.ClearData();
	for (int i = 0 ; i < VerticesNumber;i++)
	{
		Vec3f tempV;
		tempV[0] = VerticesX[i];
		tempV[1] = VerticesY[i];
		tempV[2] = VerticesZ[i];
		mesh.InsertVertex(tempV);
	}

	for (int i = 0 ; i < FaceNumber;i++)
	{
		std::vector<HE_vert* > s_faceid;
	
		HE_vert* hvA = mesh.get_vertex(FaceA[i]);
		HE_vert* hvB = mesh.get_vertex(FaceB[i]);
		HE_vert* hvC = mesh.get_vertex(FaceC[i]);
		s_faceid.push_back(hvA);
		s_faceid.push_back(hvB);
		s_faceid.push_back(hvC);
		mesh.InsertFace(s_faceid);
	}

	mesh.UpdateMesh();
}

void MeshAPI::updateVFrom(Mesh3D & mesh){
	// VerticesX, VerticesY, VerticesZ 应该在CSharp中分配空间
	for (int i = 0 ; i < VerticesNumber; i++)
	{
		point & p = mesh.get_vertex(i)->position();
		VerticesX[i] = p.x();
		VerticesY[i] = p.y();
		VerticesZ[i] = p.z();
	}
}

void MeshAPI::updateTFrom(Mesh3D & mesh){
	// TexCoordX, TexCoordY 应该在CSharp中分配空间
	for(int i=0; i<TexCoordNumber; i++){
		TexCoordX[i] = mesh.get_vertex(i)->texCoordinate().x();
		TexCoordY[i] = mesh.get_vertex(i)->texCoordinate().y();
	}
}

/*---only load V and F---*/
void MeshAPI::loadFrom(Mesh3D & mesh){
	VerticesNumber = mesh.get_vertex_list()->size();
	VerticesX = new float[VerticesNumber];
	VerticesY = new float[VerticesNumber];
	VerticesZ = new float[VerticesNumber];
	for (int i = 0 ; i < VerticesNumber; i++)
	{
		VerticesX[i] = mesh.get_vertex(i)->position().x();
		VerticesY[i] = mesh.get_vertex(i)->position().y();
		VerticesZ[i] = mesh.get_vertex(i)->position().z();
	}

	FaceNumber = mesh.get_faces_list()->size();
	FaceA = new int[FaceNumber];
	FaceB = new int[FaceNumber];
	FaceC = new int[FaceNumber];
	for (int i = 0 ; i < FaceNumber; i++)
	{
		std::vector<HE_vert *> verts;
		mesh.get_face(i)->face_verts(verts);
		FaceA[i] = verts[0]->id();
		FaceB[i] = verts[1]->id();
		FaceC[i] = verts[2]->id();
	}
}
//----
bool MeshAPIFree_CPP(MeshAPI * api){
	delete [] api->VerticesX;
	delete [] api->VerticesY;
	delete [] api->VerticesZ;
	delete [] api->FaceA;
	delete [] api->FaceB;
	delete [] api->FaceC;
	return true;
}