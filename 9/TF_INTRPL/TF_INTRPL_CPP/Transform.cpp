#include "Transform.h"
#include <iostream>
Transform::Transform()
{
	_M.setZero();
}

Transform::Transform(TF_M &t){
	_M = t;
}

Transform::Transform(Quat & q){
	_M.setIdentity();
	setR(q);
}

Transform::Transform(Quat & q, Vector4d & T){
	setR(q);
	setT(T);
	_M(3,0) = _M(3,1) = _M(3,2) = 0.0L;
}

Transform::Transform(std::vector<Vector4d> & p, std::vector<Vector4d> & Ap){
	set(p, Ap);
}

Transform::~Transform(void)
{
}

Transform Transform::R(){
	TF_M rst;
	rst.setIdentity();
	rst.block(0,0,3,3) = _M.block(0,0,3,3);
	 
	return Transform(rst);
}

Vector4d Transform::T(){
	return _M.block(0,3,4,1);
}

Quat Transform::Q(){
	Quat q;
    double m00 = _M(0, 0);
    double m11 = _M(1, 1);
    double m22 = _M(2, 2);
    q.w = 0.5L * SQRT(1 + m00 + m11 + m22);
    q.x = SIGN(_M(1, 2) - _M(2, 1)) * 0.5L * SQRT(1 + m00 - m11 - m22);
    q.y = SIGN(_M(2, 0) - _M(0, 2)) * 0.5L * SQRT(1 - m00 + m11 - m22);
    q.z = SIGN(_M(0, 1) - _M(1, 0)) * 0.5L * SQRT(1 - m00 - m11 + m22);
	return q;
}

TF_M & Transform::M(){
	return _M;
}

void Transform::setR(Quat & q){
	double q0 = q.w, q1 = q.x, q2 = q.y, q3 = q.z;

	double sqr_q0 = q0*q0;
	double sqr_q1 = q1*q1;
	double sqr_q2 = q2*q2;
	double sqr_q3 = q3*q3;

	double q0q1 = q0*q1;
	double q0q2 = q0*q2;
	double q0q3 = q0*q3;
	double q1q2 = q1*q2;
	double q1q3 = q1*q3;
	double q2q3 = q2*q3;

	_M(0, 0) = sqr_q0 + sqr_q1 - sqr_q2 - sqr_q3;
    _M(0, 1) = 2.0L*(q1q2 + q0q3);
    _M(0, 2) = 2.0L*(q1q3 - q0q2);
    _M(1, 0) = 2.0L*(q1q2 - q0q3);
    _M(1, 1) = sqr_q0 - sqr_q1 + sqr_q2 - sqr_q3;
    _M(1, 2) = 2.0L*(q2q3 + q0q1);
    _M(2, 0) = 2.0L*(q0q2 + q1q3);
    _M(2, 1) = 2.0L*(q2q3 - q0q1);
    _M(2, 2) = sqr_q0 - sqr_q1 - sqr_q2 + sqr_q3;
}

void Transform::setT(Vector4d T){
	_M.col(3) = T;
	double tmp = _M(0,3);
}

bool Transform::set(std::vector<Vector4d> & p, std::vector<Vector4d> & Ap){
	if(p.size()<3 || Ap.size()<3)
		return false;

	MatrixXd M[3];

	M[0].resize(3,5);
	M[0].setZero();
	
	for(int i=0; i<3; i++){
		M[0](i,0) = p[i](0);
		M[0](i,1) = p[i](1);
		M[0](i,2) = p[i](2);
		M[0](i,3) = p[i](3);
	}
	M[1] = M[0];
	M[2] = M[0];

	M[0](0,4) = Ap[0].x();
	M[0](1,4) = Ap[1].x();
	M[0](2,4) = Ap[2].x();

	M[1](0,4) = Ap[0].y();
	M[1](1,4) = Ap[1].y();
	M[1](2,4) = Ap[2].y();

	M[2](0,4) = Ap[0].z();
	M[2](1,4) = Ap[1].z();
	M[2](2,4) = Ap[2].z();
	
	for(int i=0; i<3; i++)
		gauss(M[i]);

	Eigen::Matrix<double,4,4> rst;
	Eigen::Matrix<double,3,3> R;
	rst.setIdentity();
	R.setZero();
	double sign[8][3] = {{-1,-1,-1},{-1,-1,1},{-1,1,-1},{-1,1,1},{1,-1,-1},{1,-1,1},{1,1,-1},{1,1,1}};
	int step;
	for(step = 0; step < 8 && ((!ISZERO(R.determinant()-1.0L) || !(R.transpose()*R).isIdentity(SMALLVALUE))); step++){
		for(int i=0; i<3; i++){
			Vector4d tmp = solve(M[i], sign[step][i]);
			rst.row(i) = tmp;
		}
		
		R = rst.block(0,0,3,3);
	}
	if(step == 8){
		return false;
	}

	_M = rst;
	return true;
}

Transform Transform::operator()(double t){
	if(t<0.0L || t>1.0L)
		return Invalid();
	
	Quat q = R().Q();
	Vector4d Tt = T();
	Tt *= t;
	Tt(3) = 1;
	return Transform(q(t), Tt);
}

Vector4d Transform::operator *(Vector4d & p){
	return _M * p;
}

Transform Transform::Invalid(){
	return Transform();
}

void Transform::gauss(MatrixXd & m){
	int rows = m.rows();
	int cols = m.cols();
	std::vector<bool> flag(rows, false);
	
	for(int i=0; i<rows; i++){
		for(int j=0; j<rows; j++){
			if(flag[j] || ISZERO(m(j, i)))
				continue;
			flag[j] = true;
			m.row(j) /= m(j,i);
			for(int k=0; k<rows; k++){
				if(k != j)
					m.row(k) -= m(k,i) * m.row(j);
			}
			break;
		}
	}
}

Vector4d Transform::solve(MatrixXd & m, double sign){
	Vector4d rst;

	if(m.rows() != 3 || m.cols() != 5)
		return Vector4d(0,0,0,0);
	
	bool zeroRow = false;
	for(int i=0; i<3; i++){
		if(ISZERO(m(i,0)) && ISZERO(m(i,1)) && ISZERO(m(i,2))){
			rst(3) = m(i,4) / m(i,3);
			zeroRow = true;
			break;
		}
	}

	if(!zeroRow){
		double a = m(0,3)*m(0,3) + m(1,3)*m(1,3) + m(2,3)*m(2,3);
		double b = -2.0L * ( m(0,3)*m(0,4) + m(1,3)*m(1,4) + m(2,3)*m(2,4) );
		double c = m(0,4)*m(0,4) + m(1,4)*m(1,4) + m(2,4)*m(2,4) - 1.0L;
		double delta = b*b - 4.0L*a*c;
		double x = (-b + sign * SQRT(delta)) / (2.0L * a);
	
		rst(3) = x;
	}

	std::vector<bool> flag(3,false);
	for(int i=0; i<3; i++){
		for(int j=0; j<3; j++){
			if(ISZERO(m(i,j) - 1.0L)){
				rst(j) = m(i,4) - m(i,3) * rst(3);
				flag[j] = true;
				break;
			}
		}
	}

	if(zeroRow){
		for(int i=0; i<3; i++){
			if(flag[i]==false){
				double tmp = 1.0L - ( pow(rst((i+1)%3), 2) + pow(rst((i+2)%3), 2) );
				rst(i) = sign * SQRT(tmp);
				break;
			}
		}
	}

	return rst;
}