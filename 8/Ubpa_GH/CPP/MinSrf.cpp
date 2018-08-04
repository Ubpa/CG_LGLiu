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
				int N = vertVec->at(i)->neighborIdx.size();
				q /= N;
				point laplace = q - vertVec->at(i)->position();
				if(len2(laplace) > delta)
					delta = len2(laplace);
				vertVec->at(i)->position() += laplace * alpha;
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
/*
void MinSrfGlobal::Minimize(Mesh3D * mesh){
	std::vector<HE_vert*> * vertVec = mesh->get_vertex_list();
    int n = 0;
    std::vector<int> idx2row;
    std::vector<int> row2idx;
    
    for(int i=0; i<vertVec->size(); i++){
        if(vertVec->at(i)->isOnBoundary()){
            idx2row.push_back(-1);
        }else{
            idx2row.push_back(n);
            row2idx.push_back(i);
            n++;
        }
    }

	SpMat A(n,n);
	std::vector<T> coef;
	for(int i=0; i<vertVec->size(); i++){
        // not on boundary
        if(idx2row[i] != -1){
            coef.push_back(T(idx2row[i], idx2row[i], 1));
            float k = 1.0 / vertVec->at(i)->neighborIdx.size();
            for(int j=0; j<vertVec->at(i)->neighborIdx.size(); j++){
                int neighborIdx = vertVec->at(i)->neighborIdx[j];
                if(idx2row[neighborIdx] != -1)
                    coef.push_back(T(idx2row[i], idx2row[neighborIdx], -k));
            }
        }
    }
    A.setFromTriplets(coef.begin(), coef.end());

    SparseLU<SpMat> solver;
    solver.compute(A);//对A进行预分解
    if(solver.info()!=Success){
        std::cout << "Compute Matrix is error" << std::endl;
        return;
    }

	Eigen::VectorXf bx(n);
	Eigen::VectorXf by(n);
	Eigen::VectorXf bz(n);
	bx*=0;
	by*=0;
	bz*=0;
	for(int i=0; i<vertVec->size(); i++){
		if(idx2row[i] != -1){
			float k = 1.0 / vertVec->at(i)->neighborIdx.size();
			for(int j=0; j<vertVec->at(i)->neighborIdx.size(); j++){
				int neighborIdx = vertVec->at(i)->neighborIdx[j];
				if(idx2row[neighborIdx] == -1){
					point & boundaryP = vertVec->at(neighborIdx)->position();
					bx(idx2row[i]) += k * boundaryP.x();
					by(idx2row[i]) += k * boundaryP.y();
					bz(idx2row[i]) += k * boundaryP.z();
				}
			}
		}
	}

	Eigen::VectorXf x = solver.solve(bx);
	Eigen::VectorXf y = solver.solve(by);
	Eigen::VectorXf z = solver.solve(bz);

    for(int i=0; i<n; i++)
        vertVec->at(row2idx[i])->position() = point(x(i), y(i), z(i));

	//mesh->UpdateMesh();
}
*/

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