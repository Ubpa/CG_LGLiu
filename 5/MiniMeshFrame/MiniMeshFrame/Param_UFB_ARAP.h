#pragma once
#include "param_ufb.h"

class Param_UFB_ARAP :
	public Param_UFB
{
public:
	void Parameterize(Mesh3D *ptr_mesh);									

private:
	float getLength(HE_vert *p, HE_vert *q);
	float getangle_p(HE_vert *p, HE_vert *q, HE_vert *r);						// get the angle opposes the q_r edge
	void set_xpoint_angle(Mesh3D *ptr_mesh);									// set the x point list and the cot value of each angle
	void set_matLlist(Mesh3D *ptr_mesh);										// set the list of matrix L 
	void set_cofmat(Mesh3D *ptr_mesh);											// set the coefficient matrix
	int get_indext(HE_vert *p, HE_vert *q, Mesh3D *ptr_mesh);					// get the index of the face where the edge pq exists
	int find_index_in_triangle(int i, int t, Mesh3D *ptr_mesh);					// find the index of the i_th vert in the t_th face
	float get_angle_oppose_edge(HE_edge *cur_vert, Mesh3D *ptr_mesh);			// get the angle opposes the given edge
	float get_cot(HE_vert *p, HE_vert *q, HE_vert *r);							

	//Eigen::Matrix2f  getJacobi(Mesh3D *stc_mesh, int cur_index);				// get the jacobi for the cur_index face
	//std::vector<std::vector<point>> x_point;									// store the initial transferred points in 2d 
	std::vector<std::vector<Eigen::Vector2f>>   x_point;
	std::vector<std::vector<float>> x_angle;									// store the angle 
	std::vector<std::vector<float>> x_angle_cot;
	//std::vector<Eigen::Matrix2f>   mat_L;
	std::vector<Eigen::Matrix2f, Eigen::aligned_allocator<Eigen::Matrix2f> > mat_L;

	SpMat cof_mat;												// set previously
	Eigen::SimplicialCholesky<SpMat>  chol;
	//Eigen::SparseLU<Eigen::SparseMatrix<SpMat>> chol;
};

