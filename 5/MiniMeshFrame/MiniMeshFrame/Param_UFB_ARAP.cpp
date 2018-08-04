#include "Param_UFB_ARAP.h"

typedef Eigen::Triplet<float> Ttr;

float Param_UFB_ARAP::getLength(HE_vert *p, HE_vert *q)
{
	float x_ = p->position_[0] - q->position_[0];
	float y_ = p->position_[1] - q->position_[1];
	float z_ = p->position_[2] - q->position_[2];
	float dis = sqrtf(powf(x_, 2) + powf(y_, 2) + powf(z_, 2));
	return dis;
}

float Param_UFB_ARAP::get_cot(HE_vert *p, HE_vert *q, HE_vert *r)
{
	//return cosf(angle_value) / sinf(angle_value);

	//get the angle opposes the edge q_r
	float temp_value = powf(getLength(p, q), 2) + powf(getLength(p, r), 2) - powf(getLength(q, r), 2);
	float cos_ = temp_value / (2.0 * getLength(p, q) * getLength(p, r));

	float cot_ = cos_ / sqrtf(1 - powf(cos_, 2));
	return cot_;
}

float Param_UFB_ARAP::getangle_p(HE_vert *p, HE_vert *q, HE_vert *r)
{
	//get the angle opposes the edge q_r
	float temp_value = powf(getLength(p, q), 2) + powf(getLength(p, r), 2) - powf(getLength(q, r), 2);
	float cos_ = temp_value / (2.0 * getLength(p, q) * getLength(p, r));

	//float cot_ = cos_ / sqrtf(1 - powf(cos_, 2));
	return acosf(cos_);
	//return cot_;
}

void Param_UFB_ARAP::set_xpoint_angle(Mesh3D *ptr_mesh)
{
	HE_vert *cur_vert0;																	// the first vert of the current face
	HE_vert *cur_vert1;
	HE_vert *cur_vert2;
	HE_edge *temp_edge;

	for (size_t t = 0; t < ptr_mesh->num_of_face_list(); t++)
	{
		temp_edge = ptr_mesh->get_face(t)->pedge_;
		cur_vert0 = temp_edge->pvert_;
		temp_edge = temp_edge->pnext_;
		cur_vert1 = temp_edge->pvert_;
		temp_edge = temp_edge->pnext_;
		cur_vert2 = temp_edge->pvert_;

		float len1 = getLength(cur_vert0, cur_vert1);
		float len2 = getLength(cur_vert1, cur_vert2);
		float len3 = getLength(cur_vert2, cur_vert0);
		std::vector<float> angle_;
		angle_.push_back(getangle_p(cur_vert2, cur_vert0, cur_vert1));
		angle_.push_back(getangle_p(cur_vert0, cur_vert1, cur_vert2));
		angle_.push_back(getangle_p(cur_vert1, cur_vert2, cur_vert0));
		//x_angle.push_back(angle_);


		
		Eigen::Vector2f p0(0.0, 0.0);
		Eigen::Vector2f p1(len1, 0.0);
		Eigen::Vector2f p2(len3 * cosf(angle_[1]), len3 * sinf(angle_[1]));
		std::vector<Eigen::Vector2f> point_;
		point_.push_back(p0);
		point_.push_back(p1);
		point_.push_back(p2);
		x_point.push_back(point_);

		angle_[0] = get_cot(cur_vert2, cur_vert0, cur_vert1);
		angle_[1] = get_cot(cur_vert0, cur_vert1, cur_vert2);
		angle_[2] = get_cot(cur_vert1, cur_vert2, cur_vert0);
		/*angle_[0] = 1.0 / tanf(angle_[0]);
		angle_[1] = 1.0 / tanf(angle_[1]);
		angle_[2] = 1.0 / tanf(angle_[2]);*/
		x_angle.push_back(angle_);
	}
}

void Param_UFB_ARAP::set_matLlist(Mesh3D *ptr_mesh)
{

	HE_vert *cur_vert0;																		// the first vert of the current face
	HE_vert *cur_vert1;
	HE_vert *cur_vert2;
	HE_edge *temp_edge;

	for (int t = 0; t < ptr_mesh->num_of_face_list(); t++)
	{
		temp_edge = ptr_mesh->get_face(t)->pedge_;
		cur_vert0 = temp_edge->pvert_;
		temp_edge = temp_edge->pnext_;
		cur_vert1 = temp_edge->pvert_;
		temp_edge = temp_edge->pnext_;
		cur_vert2 = temp_edge->pvert_;
		Eigen::Vector2f u0(cur_vert0->texCoord_[0], cur_vert0->texCoord_[1]);
		Eigen::Vector2f u1(cur_vert1->texCoord_[0], cur_vert1->texCoord_[1]);
		Eigen::Vector2f u2(cur_vert2->texCoord_[0], cur_vert2->texCoord_[1]);
		std::vector<Eigen::Vector2f> u_point;
		u_point.push_back(u0);
		u_point.push_back(u1);
		u_point.push_back(u2);

		Eigen::Matrix2f s_mat;
		s_mat.setZero();
		for (size_t i = 0; i < 3; i++)
		{
			int j = (i + 1) % 3;
			s_mat += (x_angle[t][i]) * ((u_point[i] - u_point[j]) * ((x_point[t][i] - x_point[t][j]).transpose()));
		}
		

		Eigen::JacobiSVD<Eigen::Matrix2f> svd(s_mat, Eigen::ComputeFullU | Eigen::ComputeFullV);
		Eigen::Matrix2f l_mat,u_mat,v_mat;
		u_mat = svd.matrixU();
		v_mat = svd.matrixV();
		l_mat = u_mat * (v_mat.transpose());
		mat_L.push_back(l_mat);
	}
}

void Param_UFB_ARAP::set_cofmat(Mesh3D *ptr_mesh)
{
	cof_mat.resize(ptr_mesh->num_of_vertex_list(), ptr_mesh->num_of_vertex_list());
	HE_vert *cur_vert;
	HE_vert *nei_vert;
	HE_edge *cur_edge;

	std::vector<Ttr> coefficient;
	for (int  i = 0; i < ptr_mesh->num_of_vertex_list(); i++)
	{
		cur_vert = ptr_mesh->get_vertex(i);
		float temp_i_cof = 0.0;
		for (size_t j = 0; j < cur_vert->neighborIdx.size(); j++)
		{
			float temp_j_cof = 0.0;
			nei_vert = ptr_mesh->get_vertex(cur_vert->neighborIdx[j]);
			cur_edge = ptr_mesh->get_edge(cur_vert, nei_vert);
			//if (cur_edge->isBoundary() == false)
			if (cur_edge->pface_ != NULL)
			{
				temp_j_cof += (get_angle_oppose_edge(cur_edge, ptr_mesh));
			}
			cur_edge = ptr_mesh->get_edge(nei_vert, cur_vert);
			if (cur_edge->pface_ != NULL)
			{
				temp_j_cof += (get_angle_oppose_edge(cur_edge, ptr_mesh));
			}
			coefficient.emplace_back(Ttr(i, nei_vert->id(), -1 * temp_j_cof));
			temp_i_cof += temp_j_cof;
		}
		coefficient.emplace_back(Ttr(i, i, temp_i_cof));
	}
	cof_mat.setFromTriplets(coefficient.begin(), coefficient.end());
}

float Param_UFB_ARAP::get_angle_oppose_edge(HE_edge *cur_vert, Mesh3D *ptr_mesh)
{
	int index_t = cur_vert->pface_->id();
	int index_i = cur_vert->ppair_->pvert_->id();
	index_i = find_index_in_triangle(index_i, index_t, ptr_mesh);
	return x_angle[index_t][index_i];
}

int Param_UFB_ARAP::get_indext(HE_vert *p, HE_vert *q, Mesh3D *ptr_mesh)
{																									
	HE_edge *temp_edge = ptr_mesh->get_edge(p, q);
	HE_face *temp_face = temp_edge->pface_;
	if (temp_face != NULL)
	{
		return temp_face->id_;
	}
	else
	{
		return -1;
	}
}

int Param_UFB_ARAP::find_index_in_triangle(int i, int t, Mesh3D *ptr_mesh)
{
	int index_i;																			// to find the index of the i_th vert in t_th triangle
	HE_edge *temp_edge = ptr_mesh->get_face(t)->pedge_;
	if (temp_edge->pnext_ != NULL)
	{
		if (temp_edge->pvert_->id_ == i)
		{
			index_i = 0;
		}
		else
		{
			temp_edge = temp_edge->pnext_;
			if (temp_edge->pvert_->id_ == i)
			{
				index_i = 1;
			}
			else
			{
				index_i = 2;
			}
		}
	}
	else
	{
		index_i = -1;
	}
	return index_i;
}

void Param_UFB_ARAP::Parameterize(Mesh3D *ptr_mesh){
	set_xpoint_angle(ptr_mesh);
	
	set_cofmat(ptr_mesh);
	//std::cout << cof_mat;
	chol.compute(cof_mat);

	set_matLlist(ptr_mesh);

	Eigen::VectorXf vct_x(ptr_mesh->num_of_vertex_list());
	Eigen::VectorXf vct_y(ptr_mesh->num_of_vertex_list());
	vct_x.setZero();
	vct_y.setZero();
	HE_vert *cur_vert;
	HE_vert *nei_vert;
	int index_t = -1;
	int index_i = -1;
	int index_j = -1;
	HE_edge *cur_edge;
	
	for (size_t i = 0; i < ptr_mesh->num_of_vertex_list(); i++)
	{
		cur_vert = ptr_mesh->get_vertex(i);
		Eigen::Vector2f temp_vct;
		temp_vct.setZero();
		for (size_t j = 0; j < cur_vert->neighborIdx.size(); j++)
		{
			nei_vert = ptr_mesh->get_vertex(cur_vert->neighborIdx[j]);
			//index_t = get_indext(cur_vert, nei_vert, ptr_mesh);
			cur_edge = ptr_mesh->get_edge(cur_vert, nei_vert);
			if (cur_edge->pface_ != NULL)
			{
				index_t = cur_edge->pface_->id();
				index_i = find_index_in_triangle(i, index_t, ptr_mesh);
				index_j = (index_i + 1) % 3;
				temp_vct += (get_angle_oppose_edge(cur_edge, ptr_mesh)) * mat_L[index_t] * (x_point[index_t][index_i] - x_point[index_t][index_j]);
			}

			
			cur_edge = ptr_mesh->get_edge(nei_vert, cur_vert);
			if (cur_edge->pface_ != NULL)
			{
				index_t = cur_edge->pface_->id();
				index_j = find_index_in_triangle(nei_vert->id(), index_t, ptr_mesh);
				index_i = (index_j + 1) % 3;
				temp_vct += (get_angle_oppose_edge(cur_edge, ptr_mesh)) * mat_L[index_t] * (x_point[index_t][index_i] - x_point[index_t][index_j]);
			}
			/*index_t = get_indext(nei_vert, cur_vert, ptr_mesh);
			if (index_t != -1)
			{
				index_j = find_index_in_triangle(cur_vert->neighborIdx[j], index_t, ptr_mesh);
				index_i = (index_j + 1) % 3;
				temp_vct += x_angle[index_t][index_j] * mat_L[index_t] * (x_point[index_t][index_j] - x_point[index_t][index_i]);
			}*/
			
		}
		vct_x(i) = temp_vct(0);
		vct_y(i) = temp_vct(1);
	}

	Eigen::VectorXf cor_x(ptr_mesh->num_of_vertex_list());
	cor_x = chol.solve(vct_x);
	Eigen::VectorXf cor_y(ptr_mesh->num_of_vertex_list());
	cor_y = chol.solve(vct_y);
	for (int i = 0; i < ptr_mesh->num_of_vertex_list(); i++)
	{
		cur_vert = ptr_mesh->get_vertex(i);
		cur_vert->texCoord_[0] = cor_x(i);
		cur_vert->texCoord_[1] = cor_y(i);
	}
	
}