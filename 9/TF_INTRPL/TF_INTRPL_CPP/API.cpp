#include "API.h"
#include <ctime>

bool TF_Interpolation_CPP(API * api){
	bool done = false;
	Transform tf;
	srand(9704);
	for(int cnt = 0; cnt < 100; cnt++){
		//存在精度问题，故多次尝试，直到成功
		// step1: 选取三个点
		std::vector<Vector4d> p(3), Ap(3);
		std::vector<int> idxV;
		for(int i=0; i<3; i++){
			int idx;
			while(true){
				idx = rand()%api->num;
				bool same = false;
				for(int j=0; j<idxV.size(); j++){
					if(idxV[i] == idx){
						same = true;
						break;
					}
				}
				if(!same)
					break;
			}
			p[i] = Vector4d(api->px[idx], api->py[idx], api->pz[idx], 1.0L);
			Ap[i] = Vector4d(api->Apx[idx], api->Apy[idx], api->Apz[idx], 1.0L);
		}
		// step 2：尝试找到变换矩阵
		if(tf.set(p, Ap)){
			done = true;
			break;
		}
	}
	if(!done)
		return false;

	// A(t)
	Transform tf_t = tf(api->t);
	for(int i=0; i<api->num; i++){
		// A(t) * pi
		Vector4d Ap_i_t = tf_t * Vector4d(api->px[i], api->py[i], api->pz[i], 1.0L);
		api->Apx[i] = Ap_i_t.x();
		api->Apy[i] = Ap_i_t.y();
		api->Apz[i] = Ap_i_t.z();
	}
	
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			api->transform[i*4+j] = tf_t.M()(i,j);
		}
	}
	
	return true;
}