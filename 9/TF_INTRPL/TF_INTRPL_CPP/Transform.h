#include "Quat.h"

class Transform
{
public:
	Transform();
	Transform(Quat & q);
	Transform(Quat & q, Vector4d & T);
	Transform(TF_M & t);
	Transform(std::vector<Vector4d> & p, std::vector<Vector4d> & Ap);
	~Transform();

	Transform R();
	Vector4d T();
	Quat Q();
	TF_M & M();

	void setR(Quat & q);
	void setT(Vector4d T);
	bool set(std::vector<Vector4d> & p, std::vector<Vector4d> & Ap);

	Transform operator ()(double t);
	Vector4d operator *(Vector4d & p);

	static Transform Invalid();
private:
	TF_M _M;
	static void gauss(MatrixXd & m);
	static Vector4d solve(MatrixXd & m, double sign);
};

