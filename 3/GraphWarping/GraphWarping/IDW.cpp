#include "IDW.h"

IDW::IDW(std::vector<ND_Point> & _pVec, std::vector<ND_Point> & _fpVec, float _u){
	u = _u;

	if( _pVec.size() != _fpVec.size() )
		return;
	for(int i=0; i < _pVec.size(); i++){
		pVec.push_back(_pVec[i]);
		fpVec.push_back(_fpVec[i]);
	}
}

IDW::~IDW(){
}

ND_Point IDW::map(ND_Point & p){
	if(pVec.size() == 0)
		exit(1);
	std::vector<float> simVec;// 相似度向量
	float sumSim = 0, curSim;
	for(int i=0; i < pVec.size(); i++){
		curSim = pow((pVec[i] - p).mod2(), -u/2);
		sumSim += curSim;
		simVec.push_back(curSim);
	}
	
	float pD[3] = {0};
	int dim = fpVec[0].dim;
	for(int i=0; i < pVec.size(); i++){
		float w = simVec[i] / sumSim;
		ND_Point & fp = fpVec[i];
		pD[0] += fp.x * w;
		pD[1] += fp.y * w;
		if(dim==3)
			pD[2] += fp.z * w;
	}
	
	if(dim==3)
		return ND_Point(pD[0], pD[1], pD[2]);
	else
		return ND_Point(pD[0], pD[1]);
}