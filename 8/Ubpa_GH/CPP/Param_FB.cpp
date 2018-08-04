#include "Param_FB.h"

void Param_FB::Parameterize(Mesh3D * ptr_mesh){
	getBoundary(ptr_mesh);
	setBoundary(ptr_mesh);
	mapToRegion2D(ptr_mesh);
}

void Param_FB::getBoundary(Mesh3D * ptr_mesh){
	HE_vert *cur_vert = NULL;
	HE_vert *nei_vert = NULL;

	// find the first boundary point
	for(int i = 0; i < ptr_mesh->num_of_vertex_list(); i++){																		
		cur_vert = ptr_mesh->get_vertex(i);
		if(cur_vert->isOnBoundary() == true){
			boundary.push_back(i);
			break;
		}
	}

	while(true){
		for(int i = 0; i < cur_vert->neighborIdx.size(); i++){
			nei_vert = ptr_mesh->get_vertex(cur_vert->neighborIdx[i]);
			if(nei_vert->isOnBoundary() == true){
				boundary.push_back(nei_vert->id());
				cur_vert = nei_vert;
				break;
			}
		}
		if(cur_vert->id() == boundary[0])
			break;
	}
}

void Param_FB::setBoundary(Mesh3D * ptr_mesh){
	float step_length = 4.0 / boundary.size();
	float temp_value = step_length;
	for(int i = 0; i < boundary.size(); i++, temp_value += step_length){
		switch(i*4/boundary.size()){
		case 0:
			ptr_mesh->get_vertex(boundary[i])->texCoord_[0] = temp_value;
			ptr_mesh->get_vertex(boundary[i])->texCoord_[1] = 0.0;
			break;
		case 1:
			ptr_mesh->get_vertex(boundary[i])->texCoord_[0] = 1.0;
			ptr_mesh->get_vertex(boundary[i])->texCoord_[1] = temp_value - 1.0;
			break;
		case 2:
			ptr_mesh->get_vertex(boundary[i])->texCoord_[0] = 3.0 - temp_value;
			ptr_mesh->get_vertex(boundary[i])->texCoord_[1] = 1.0;
			break;
		case 3:
			ptr_mesh->get_vertex(boundary[i])->texCoord_[0] = 0.0;
			ptr_mesh->get_vertex(boundary[i])->texCoord_[1] = 4.0 - temp_value;
			break;
		default:
			break;
		}
	}	
}

void Param_FB::mapToRegion2D(Mesh3D * ptr_mesh){
	// find inner points
    std::vector<int> idx2row, row2idx;
    int n = 0;
    for(int i=0; i<ptr_mesh->get_vertex_list()->size(); i++){
        if(ptr_mesh->get_vertex(i)->isOnBoundary())
            idx2row.push_back(-1);
        else{
            idx2row.push_back(n);
            row2idx.push_back(i);
            n++;
        }
    }
	
	// fill A and b
	SpMat A(n, n);
	MatrixXf b;
	b.setZero(n, 2);

	std::vector<T> coef;
	for(int i=0; i<ptr_mesh->get_vertex_list()->size(); i++){
        // not on boundary
        if(idx2row[i] != -1){
            coef.push_back(T(idx2row[i], idx2row[i], 1));
			// getWeights is a virtual function
			std::vector<float> weights = getWeights(ptr_mesh->get_vertex(i), ptr_mesh);
            for(int j=0; j<ptr_mesh->get_vertex(i)->neighborIdx.size(); j++){
                int neighborIdx = ptr_mesh->get_vertex(i)->neighborIdx[j];
                if(idx2row[neighborIdx] != -1)
                    coef.push_back(T(idx2row[i], idx2row[neighborIdx], -weights[j]));
				else{
					b(idx2row[i], 0) += weights[j] * ptr_mesh->get_vertex(neighborIdx)->texCoord_[0];
					b(idx2row[i], 1) += weights[j] * ptr_mesh->get_vertex(neighborIdx)->texCoord_[1];
				}
            }
        }
    }
    A.setFromTriplets(coef.begin(), coef.end());

	// 对A进行预分解
    SparseLU<SpMat> solver;
    solver.compute(A);
    if(solver.info()!=Success){
        std::cout << "Compute Matrix is error" << std::endl;
        return;
    }

	// A * X = b ====> X
	MatrixXf X = solver.solve(b);
	for (int i = 0; i < n; i++){
		ptr_mesh->get_vertex(row2idx[i])->texCoord_[0] = X(i, 0);
		ptr_mesh->get_vertex(row2idx[i])->texCoord_[1] = X(i, 1);
	}
}
