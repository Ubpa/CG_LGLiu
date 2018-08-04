#include "RBF.h"

RBF::RBF(std::vector<QPoint *> *_pVec, std::vector<QPoint *> *_qVec){
	r = 25;
	u = 1;

	const int N = _pVec->size();
	if(N != _qVec->size())
		return;
	for(int i=0; i < N; i++){
		pVec.push_back(new QPoint(*(_pVec->at(i))));
		qVec.push_back(new QPoint(*(_qVec->at(i))));
	}
	
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
			QPoint diff = *_pVec->at(i) - *_pVec->at(j);
			A(i,j) = pow(pow((double)diff.x(),2)+pow((double)diff.y(), 2) + r*r, u/2.0);
		}
	}

	// H : N x 3
	for(int i=0; i<N; i++){
		for(int j=0; j<3; j++){
			A(i, j+N) = ( j==0 ? _pVec->at(i)->x()
					  : ( j==1 ? _pVec->at(i)->y()
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
		b(i) = _qVec->at(i)->x();
	RBF_A = new MatrixXd(2, N+3);
	VectorXd x = A.colPivHouseholderQr().solve(b);
	for(int i=0; i<N+3; i++)
		(*RBF_A)(0, i) = x(i);

	for(int i=0; i<N; i++)
		b(i) = _qVec->at(i)->y();
	VectorXd y = A.colPivHouseholderQr().solve(b);
	for(int i=0; i<N+3; i++)
		(*RBF_A)(1, i) = y(i);
}


RBF::~RBF(){
	for(int i=0; i < pVec.size(); i++)
		delete pVec[i], qVec[i];
	
	delete RBF_A;
}

QPoint RBF::map(QPoint p){
	// x = [d, p, 1]
	const int N = pVec.size();
	MatrixXd x(N+3,1);
	for(int i=0; i<N; i++){
		QPoint diff = p - *pVec[i];
		x(i,0) = pow(pow((double)diff.x(),2)+pow((double)diff.y(), 2) + r*r, u/2.0);
	}
	x(N,0) = p.x();
	x(N+1,0) = p.y();
	x(N+2,0) = 1;

	MatrixXd rst = (*RBF_A) * x;
	return QPoint(rst(0,0), rst(1,0));
}