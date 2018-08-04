#include "MinSrf.h"

void MinSrfLocal::Minimize(Mesh3D * mesh){
	std::vector<HE_vert*> * vertVec = mesh->get_vertex_list();
	float alpha = 0.2;
	float epsilon = 0.000001;
	int cnt;
	while(true){
		cnt = 0;
		float delta = 0;
		for(int i=0; i<vertVec->size(); i++){
			if(!vertVec->at(i)->isOnBoundary()){
				cnt++;
				point q;
				for(int j=0; j<vertVec->at(i)->neighborIdx.size(); j++)
					q += vertVec->at(vertVec->at(i)->neighborIdx[j])->position();
				q /= vertVec->at(i)->neighborIdx.size();
				point laplace = q - vertVec->at(i)->position();
				if(len2(laplace) > delta)
					delta = len2(laplace);
				vertVec->at(i)->position() += alpha * laplace;
			}
		}
		if(delta < epsilon)
			break;
	}
	mesh->UpdateMesh();
}

void MinSrfGlobal::Minimize(Mesh3D * mesh){
	auto ptr_pointlist = mesh->get_vertex_list();
	int n = ptr_pointlist->size();
	SpMat coefficient;
	coefficient.resize(n,n);
	VectorXf Ex;
	VectorXf Ey;
	VectorXf Ez;
	Ex.resize(n);
	Ey.resize(n);
	Ez.resize(n);
	Ex.setZero();
	Ey.setZero();
	Ez.setZero();
	std::vector<T> triplets;
	for (size_t i = 0;i<n;i++)
	{
		if((*ptr_pointlist)[i]->isOnBoundary())
		{
			Ex(i) = (*ptr_pointlist)[i]->position_.x();
			Ey(i) = (*ptr_pointlist)[i]->position_.y();
			Ez(i) = (*ptr_pointlist)[i]->position_.z();
			triplets.emplace_back(T(i,i,1.0));		// 初始化非零元素
		}
		else{
			int NeighbourCount = 0;
			HE_edge *TempEdge = (*ptr_pointlist)[i]->pedge_;
			do{
				TempEdge = TempEdge->ppair_->pnext_;
				triplets.emplace_back(T(i,TempEdge->pvert_->id(),1));
				NeighbourCount++;
			} while (TempEdge != (*ptr_pointlist)[i]->pedge_);
			triplets.emplace_back(T(i,i,-NeighbourCount));
		}

	}
	coefficient.setFromTriplets(triplets.begin(),triplets.end());

	BiCGSTAB< SpMat > solver(coefficient);

	//SparseQR< SparseMatrix<double>,AMDOrdering<int> > qr;
	//qr.compute(coefficient);//速度太慢


	VectorXf Newx(n);
	VectorXf Newy(n);
	VectorXf Newz(n);

	Newx = solver.solve(Ex);
	Newy = solver.solve(Ey);
	Newz = solver.solve(Ez);
	for (size_t i = 0; i < n ;i++)
	{
		if ((*ptr_pointlist)[i]->isOnBoundary())
		{
			continue;
		}
		(*ptr_pointlist)[i]->set_position(Vec3f(Newx(i),Newy(i),Newz(i)));
	}
}

void MinSrfCurve::Minimize(std::vector<point> & curve, Mesh3D * mesh){
	mesh->ClearData();
	
	std::vector<point2> V;
	int n = (curve.size()+3)/4;
	// 边界
	for(int i=1; i<=n; i++)
		V.push_back(point2(0,i));
	for(int i=1; i<=n; i++)
		V.push_back(point2(i,n+1));
	if(curve.size()!=5){
		for(int i=n; i>=1; i--)
			V.push_back(point2(n+1,i));
		for(int i=n; i>4*n-curve.size(); i--)
			V.push_back(point2(i,0));
	}else{
		// 特殊情况
		V.push_back(point2(3,1));
	}
	
	// 内部
	for(int i=1; i<=n; i++){
		for(int j=1; j<=n; j++){
			V.push_back(point2(i,j));
		}
	}

	// gen face
	std::vector<std::vector<int>> faces;
	genTriMesh(V, faces);

	// gen mesh
	for (int i = 0 ; i < curve.size(); i++)
		mesh->InsertVertex(Vec3f(curve[i].x(),curve[i].y(),curve[i].z()));
	for (int i = curve.size(); i < V.size(); i++)
		mesh->InsertVertex(Vec3f(V[i].x(),V[i].y(),0.0f));

	for (int i = 0 ; i < faces.size();i++){
		std::vector<HE_vert* > s_faceid;
	
		HE_vert* hvA = mesh->get_vertex(faces[i][0]);
		HE_vert* hvB = mesh->get_vertex(faces[i][1]);
		HE_vert* hvC = mesh->get_vertex(faces[i][2]);
		s_faceid.push_back(hvA);
		s_faceid.push_back(hvB);
		s_faceid.push_back(hvC);
		mesh->InsertFace(s_faceid);
	}
	mesh->UpdateMesh();
	MinSrfGlobal::Minimize(mesh);
}
//-------------------------------------------------
void genTriMesh(std::vector<point2> & V, std::vector<std::vector<int>> & faces){
	faces.clear();
	struct triangulateio in, out;

	// 初始化 in
	in.numberofpoints = V.size();
	in.pointlist = new double[2 * in.numberofpoints];
	in.numberofpointattributes = 1;
	in.pointattributelist = new double[in.numberofpoints * in.numberofpointattributes];
	in.pointmarkerlist = new int[in.numberofpoints];
	for (int i = 0; i < in.numberofpoints; i++) {
		in.pointlist[i * 2] = V[i].x();
		in.pointlist[i * 2 + 1] = V[i].y();
		in.pointattributelist[i] = 10;
		in.pointmarkerlist[i] = 1;
	}
	in.numberofsegments = 0;
	in.numberofholes = 0;
	in.numberofregions = 0;

	// 初始化 out
	out.pointlist = (TRI_REAL *)NULL;
	out.pointattributelist = (TRI_REAL *)NULL;
	out.pointmarkerlist = (int *)NULL;
	out.trianglelist = (int *)NULL;
	out.triangleattributelist = (TRI_REAL *)NULL;
	out.neighborlist = (int *)NULL;
	out.segmentlist = (int *)NULL;
	out.segmentmarkerlist = (int *)NULL;
	out.edgelist = (int *)NULL;
	out.edgemarkerlist = (int *)NULL;

	// 三角化
	triangulate("pczAenQ", &in, &out, (struct triangulateio *) NULL);

	// get faces
	for(int i = 0; i< out.numberoftriangles; i++){
		std::vector<int> face;
		for(int j = 0; j < out.numberofcorners; j++)
			face.push_back(out.trianglelist[i * out.numberofcorners + j]);
		faces.push_back(face);
	}

	// free
	free(in.pointlist);
	free(in.pointattributelist);
	free(in.pointmarkerlist);
	free(out.pointlist);
	free(out.pointattributelist);
	free(out.pointmarkerlist);
	free(out.trianglelist);
	free(out.triangleattributelist);
	free(out.neighborlist);
	free(out.segmentlist);
	free(out.segmentmarkerlist);
	free(out.edgelist);
	free(out.edgemarkerlist);
}