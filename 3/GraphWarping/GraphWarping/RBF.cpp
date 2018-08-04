#include "RBF.h"

RBF::RBF(std::vector<ND_Point> & _pVec, std::vector<ND_Point> & _fpVec, float _r, float _u){
	r = _r;
	u = _u;
	const int N = _pVec.size();

	if(N != _fpVec.size())
		exit(1);
	pVec.assign(_pVec.begin(), _pVec.end());
	fpVec.assign(_fpVec.begin(), _fpVec.end());
	
	// A = -       -
	//     | G   H |
	//     |       |
	//     |  t    |
	//     | H   0 |
	//     -       -
	MatrixXd A(N+3, N+3);

	// G : N x N
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++){
			ND_Point diff = _pVec[i] - _pVec[j];
			A(i,j) = pow(diff.mod2() + r*r, u/2);
		}
	}

	// H : N x 3
	for(int i=0; i<N; i++){
		for(int j=0; j<3; j++){
			A(i, j+N) = ( j==0 ? _pVec[i].x
					  : ( j==1 ? _pVec[i].y
					  : 1 ));
			A(j+N, i) = A(i, j+N);
		}
	}
	
	// 0
	for(int i=N; i<N+3; i++){
		for(int j=N; j<N+3; j++){
			A(i,j) = 0;
		}
	}

	// b
	VectorXd b(N+3);
	for(int i=N; i<N+3; i++)
		b(i) = 0;
	
	// RBF_A = [a, alpha]
	for(int i=0; i<N; i++)
		b(i) = _fpVec[i].x;
	RBF_A = new MatrixXd(2, N+3);
	VectorXd x = A.colPivHouseholderQr().solve(b);
	for(int i=0; i<N+3; i++)
		(*RBF_A)(0, i) = x(i);

	for(int i=0; i<N; i++)
		b(i) = _fpVec[i].y;
	VectorXd y = A.colPivHouseholderQr().solve(b);
	for(int i=0; i<N+3; i++)
		(*RBF_A)(1, i) = y(i);
}

RBF::~RBF(){
	delete RBF_A;
}

ND_Point RBF::map(ND_Point & p){
	// x = [d, p, 1]
	const int N = pVec.size();
	MatrixXd x(N+3,1);
	for(int i=0; i<N; i++){
		ND_Point diff = p - pVec[i];
		x(i,0) = pow((float)(diff.mod2() + r*r), u/2.0f);
	}
	x(N,0) = p.x;
	x(N+1,0) = p.y;
	x(N+2,0) = 1;

	MatrixXd rst = (*RBF_A) * x;
	return ND_Point(rst(0,0), rst(1,0));
}