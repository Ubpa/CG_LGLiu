#include "renderingwidget.h"
#include <QtGui\QKeyEvent>
#include <QColorDialog>
#include <QFileDialog>
#include <iostream>
#include <QMenu>
#include <QAction>
#include <QTextCodec>
#include <gl\GLU.h>
#include <gl\glut.h>
#include <algorithm>
#include "mainwindow.h"
#include "ArcBall.h"
#include "globalFunctions.h"
#include "HE_mesh/Mesh3D.h"
#include <Dense>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <Sparse>
using namespace Eigen;
typedef Eigen::SparseMatrix<double> SpMat; // 声明一个列优先的双精度稀疏矩阵类型
typedef Eigen::Triplet<double> T; //三元组（行，列，值）
#define PI 3.1415926
#define START() clock_t start_time = clock()
#define END(str) \
clock_t end_time = clock();\
std::cout<< (str) << " : "\
	<<static_cast<float>(end_time-start_time)/CLOCKS_PER_SEC*1000\
	<<"ms"<<std::endl;//输出运行时间

RenderingWidget::RenderingWidget(QWidget *parent, QEvent::Type * _eventType, MainWindow* mainwindow)
: QGLWidget(parent), ptr_mainwindow_(mainwindow), eye_distance_(5.0),
has_lighting_(false), is_draw_point_(true), is_draw_edge_(true), is_draw_face_(false), is_draw_texture_(false)
{
	parent_ = parent;
	ptr_arcball_ = new CArcBall(width(), height());
	cur_mesh_ = new Mesh3D();

	is_load_texture_ = false;
	is_draw_axes_ = false;

	eye_goal_[0] = eye_goal_[1] = eye_goal_[2] = 0.0;
	eye_direction_[0] = eye_direction_[1] = 0.0;
	eye_direction_[2] = 1.0;

	for(int i=0; i<2*TOOLNUM; i++)
		eventType[i] = _eventType[i];
}

RenderingWidget::~RenderingWidget()
{
	SafeDelete(ptr_arcball_);
	SafeDelete(cur_mesh_);
	for(int i=0; i<mesh_vec_.size(); i++){
		mesh_vec_[i]->ClearData();
		SafeDelete(mesh_vec_[i]);
	}
}

void RenderingWidget::initializeGL()
{
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DOUBLEBUFFER);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1);

	SetLight();

}

void RenderingWidget::resizeGL(int w, int h)
{
	h = (h == 0) ? 1 : h;

	ptr_arcball_->reSetBound(w, h);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, GLdouble(w) / GLdouble(h), 0.001, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderingWidget::paintGL()
{
	glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (has_lighting_)
	{
		SetLight();
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	register vec eyepos = eye_distance_*eye_direction_;
	gluLookAt(eyepos[0], eyepos[1], eyepos[2],
		eye_goal_[0], eye_goal_[1], eye_goal_[2],
		0.0, 1.0, 0.0);
	glPushMatrix();

	glMultMatrixf(ptr_arcball_->GetBallMatrix());

	Render();
	glPopMatrix();
}

void RenderingWidget::timerEvent(QTimerEvent * e)
{
	updateGL();
}

void RenderingWidget::mousePressEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		ptr_arcball_->MouseDown(e->pos());
		break;
	case Qt::MidButton:
		current_position_ = e->pos();
		break;
	default:
		break;
	}

	updateGL();
}

void RenderingWidget::mouseMoveEvent(QMouseEvent *e)
{
	switch (e->buttons())
	{
		setCursor(Qt::ClosedHandCursor);
	case Qt::LeftButton:
		ptr_arcball_->MouseMove(e->pos());
		break;
	case Qt::MidButton:
		eye_goal_[0] -= 4.0*GLfloat(e->x() - current_position_.x()) / GLfloat(width());
		eye_goal_[1] += 4.0*GLfloat(e->y() - current_position_.y()) / GLfloat(height());
		current_position_ = e->pos();
		break;
	default:
		break;
	}

	updateGL();
}

void RenderingWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		break;
	default:
		break;
	}
	updateGL();
}

void RenderingWidget::mouseReleaseEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		ptr_arcball_->MouseUp(e->pos());
		setCursor(Qt::ArrowCursor);
		break;

	case Qt::RightButton:
		break;
	default:
		break;
	}
}

void RenderingWidget::wheelEvent(QWheelEvent *e)
{
	eye_distance_ += e->delta()*0.001;
	eye_distance_ = eye_distance_ < 0 ? 0 : eye_distance_;

	updateGL();
}

void RenderingWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_A:
		break;
	default:
		break;
	}
}

void RenderingWidget::keyReleaseEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_A:
		break;
	default:
		break;
	}
}

void RenderingWidget::Render()
{
	DrawAxes(is_draw_axes_);

	DrawPoints(is_draw_point_);
	DrawEdge(is_draw_edge_);
	DrawFace(is_draw_face_);
	DrawTexture(is_draw_texture_);
}

void RenderingWidget::SetLight()
{
	static GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat mat_shininess[] = { 50.0 };
	static GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	static GLfloat white_light[] = { 0.8, 0.8, 0.8, 1.0 };
	static GLfloat lmodel_ambient[] = { 0.3, 0.3, 0.3, 1.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void RenderingWidget::SetBackground()
{
	QColor color = QColorDialog::getColor(Qt::white, this, tr("background color"));
	GLfloat r = (color.red()) / 255.0f;
	GLfloat g = (color.green()) / 255.0f;
	GLfloat b = (color.blue()) / 255.0f;
	GLfloat alpha = color.alpha() / 255.0f;
	glClearColor(r, g, b, alpha);
	updateGL();
}

void RenderingWidget::ReadMesh()
{
	QString filename = QFileDialog::
		getOpenFileName(this, tr("Read Mesh"),
		"..", tr("Meshes (*.obj)"));

	if(ReadMeshFrom(filename)){
		updateGL();
	
		SW(TOOL::TEX, ACTIVATE);
		SW(TOOL::SAVEAS, ACTIVATE);
		SW(TOOL::SAVEAS, INACTIVATE);

		for(int i=0; i<mesh_vec_.size(); i++){
			mesh_vec_.back()->ClearData();
			mesh_vec_.pop_back();
		}

		if(cur_mesh_->get_vertex_list()->size() == (cur_mesh_->get_edges_list()->size()/2)){
			SW(TOOL::MINSURF_CURVE, ACTIVATE);
			SW(TOOL::MINSURF_ITER, INACTIVATE);
			SW(TOOL::MINSURF_FORM, INACTIVATE);
		}else{
			SW(TOOL::MINSURF_CURVE, INACTIVATE);
			SW(TOOL::MINSURF_ITER, ACTIVATE);
			SW(TOOL::MINSURF_FORM, ACTIVATE);
		}
	}
}

bool RenderingWidget::ReadMeshFrom(QString & filename){
	std::cout << std::string((const char *)filename.toLocal8Bit()) << std::endl;

	if (filename.isEmpty())
	{
		emit(operatorInfo(QString("Read Mesh Failed!")));
		return false;
	}
	//中文路径支持
	QTextCodec *code = QTextCodec::codecForName("gd18030");
	QTextCodec::setCodecForLocale(code);

	QByteArray byfilename = filename.toLocal8Bit();
	cur_mesh_->LoadFromOBJFile(byfilename.data());

	//	m_pMesh->LoadFromOBJFile(filename.toLatin1().data());
	emit(operatorInfo(QString("Read Mesh from") + filename + QString(" Done")));
	emit(meshInfo(cur_mesh_->num_of_vertex_list(), cur_mesh_->num_of_edge_list(), cur_mesh_->num_of_face_list()));
	
	return true;
}

void RenderingWidget::WriteMesh()
{
	if (cur_mesh_->num_of_vertex_list() == 0)
	{
		emit(QString("The Mesh is Empty !"));
		return;
	}
	QString filename = QFileDialog::
		getSaveFileName(this, tr("Write Mesh"),
		"..", tr("Meshes (*.obj)"));

	if (filename.isEmpty())
		return;

	cur_mesh_->WriteToOBJFile(filename.toLatin1().data());

	emit(operatorInfo(QString("Write Mesh to ") + filename + QString(" Done")));
}

void RenderingWidget::LoadTexture()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Texture"),
		"..", tr("Images(*.bmp *.jpg *.png *.jpeg)"));
	if (filename.isEmpty())
	{
		emit(operatorInfo(QString("Load Texture Failed!")));
		return;
	}


	glGenTextures(1, &texture_[0]);
	QImage tex1, buf;
	if (!buf.load(filename))
	{
		//        QMessageBox::warning(this, tr("Load Fialed!"), tr("Cannot Load Image %1").arg(filenames.at(0)));
		emit(operatorInfo(QString("Load Texture Failed!")));
		return;
		/*
		QImage dummy(128, 128, QImage::Format_ARGB32);
		dummy.fill(Qt::green);
		buf = dummy;
		*/
	}

	tex1 = QGLWidget::convertToGLFormat(buf);
	glBindTexture(GL_TEXTURE_2D, texture_[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, tex1.width(), tex1.height(),
		GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits());

	is_load_texture_ = true;
	emit(operatorInfo(QString("Load Texture from ") + filename + QString(" Done")));
}

void RenderingWidget::CheckDrawPoint(bool bv)
{
	is_draw_point_ = bv;
	updateGL();
}

void RenderingWidget::CheckDrawEdge(bool bv)
{
	is_draw_edge_ = bv;
	updateGL();
}

void RenderingWidget::CheckDrawFace(bool bv)
{
	is_draw_face_ = bv;
	updateGL();
}

void RenderingWidget::CheckLight(bool bv)
{
	has_lighting_ = bv;
	updateGL();
}

void RenderingWidget::CheckDrawTexture(bool bv)
{
	is_draw_texture_ = bv;
	if (is_draw_texture_)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	updateGL();
}

void RenderingWidget::CheckDrawAxes(bool bV)
{
	is_draw_axes_ = bV;
	updateGL();
}

void RenderingWidget::DrawAxes(bool bV)
{
	if (!bV)
		return;
	//x axis
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.7, 0.0, 0.0);
	glEnd();
	glPushMatrix();
	glTranslatef(0.7, 0, 0);
	glRotatef(90, 0.0, 1.0, 0.0);
	glutSolidCone(0.02, 0.06, 20, 10);
	glPopMatrix();

	//y axis
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.0, 0.7, 0.0);
	glEnd();

	glPushMatrix();
	glTranslatef(0.0, 0.7, 0);
	glRotatef(90, -1.0, 0.0, 0.0);
	glutSolidCone(0.02, 0.06, 20, 10);
	glPopMatrix();

	//z axis
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.0, 0.0, 0.7);
	glEnd();
	glPushMatrix();
	glTranslatef(0.0, 0, 0.7);
	glutSolidCone(0.02, 0.06, 20, 10);
	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);
}

void RenderingWidget::DrawPoints(bool bv)
{
	if (!bv || cur_mesh_ == NULL)
		return;
	if (cur_mesh_->num_of_vertex_list() == 0)
	{
		return;
	}

	const std::vector<HE_vert*>& verts = *(cur_mesh_->get_vertex_list());
	glBegin(GL_POINTS);
	for (size_t i = 0; i != cur_mesh_->num_of_vertex_list(); ++i)
	{
		glNormal3fv(verts[i]->normal().data());
		glVertex3fv(verts[i]->position().data());
	}
	glEnd();
}

void RenderingWidget::DrawEdge(bool bv)
{
	if (!bv || cur_mesh_ == NULL)
		return;

	if (cur_mesh_->num_of_face_list() == 0)
	{
		return;
	}

	const std::vector<HE_face *>& faces = *(cur_mesh_->get_faces_list());
	for (size_t i = 0; i != faces.size(); ++i)
	{
		glBegin(GL_LINE_LOOP);
		HE_edge *pedge(faces.at(i)->pedge_);
		do
		{
			glNormal3fv(pedge->pvert_->normal().data());
			glVertex3fv(pedge->pvert_->position().data());

			pedge = pedge->pnext_;

		} while (pedge != faces.at(i)->pedge_);
		glEnd();
	}
}

void RenderingWidget::DrawFace(bool bv)
{
	if (!bv || cur_mesh_ == NULL)
		return;

	if (cur_mesh_->num_of_face_list() == 0)
	{
		return;
	}

	const std::vector<HE_face *>& faces = *(cur_mesh_->get_faces_list());

	glBegin(GL_TRIANGLES);

	for (size_t i = 0; i != faces.size(); ++i)
	{
		HE_edge *pedge(faces.at(i)->pedge_);
		do
		{
			glNormal3fv(pedge->pvert_->normal().data());
			glVertex3fv(pedge->pvert_->position().data());

			pedge = pedge->pnext_;

		} while (pedge != faces.at(i)->pedge_);
	}

	glEnd();
}

void RenderingWidget::DrawTexture(bool bv)
{
	if (!bv)
		return;
	if (cur_mesh_->num_of_face_list() == 0 || !is_load_texture_)
		return;

	//默认使用球面纹理映射，效果不好
	cur_mesh_->SphereTex();

	const std::vector<HE_face *>& faces = *(cur_mesh_->get_faces_list());

	glBindTexture(GL_TEXTURE_2D, texture_[0]);
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i != faces.size(); ++i)
	{
		HE_edge *pedge(faces.at(i)->pedge_);
		do
		{
			/*请在此处绘制纹理，添加纹理坐标即可*/
			glTexCoord2fv(pedge->pvert_->texCoord_.data());
			glNormal3fv(pedge->pvert_->normal().data());
			glVertex3fv(pedge->pvert_->position().data());

			pedge = pedge->pnext_;

		} while (pedge != faces.at(i)->pedge_);
	}

	glEnd();
}

void RenderingWidget::Iteration(){
#ifdef ENABLE_UNDO
	cur_mesh_->WriteToOBJFile("tmp.obj");
	mesh_vec_.push_back(cur_mesh_);
	cur_mesh_ = new Mesh3D;
	cur_mesh_->LoadFromOBJFile("tmp.obj");
#endif
	START();

    std::vector<HE_vert*> * vertVec = cur_mesh_->get_vertex_list();
	float alpha = 0.2;
	float epsilon = 0.00000001;
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
				q /= vertVec->at(i)->neighborIdx.size();
				point laplace = q - vertVec->at(i)->position();
				if(len2(laplace) > delta)
					delta = len2(laplace);
				vertVec->at(i)->position() += alpha * laplace;
			}
		}
		if(delta < epsilon)
			break;
		updateGL();
	}
	printf("Boundary : %d, Inner: %d\n", vertVec->size() - cnt, cnt);
	END("Time of Iteration");
	
	SW(TOOL::UNDO, ACTIVATE);
	SW(TOOL::MINSURF_ITER, INACTIVATE);
}

void RenderingWidget::Formula(){
#ifdef ENABLE_UNDO
	cur_mesh_->WriteToOBJFile("tmp.obj");
	mesh_vec_.push_back(cur_mesh_);
	cur_mesh_ = new Mesh3D();
	cur_mesh_->LoadFromOBJFile("tmp.obj");
#endif
	START();

    std::vector<HE_vert*> * vertVec = cur_mesh_->get_vertex_list();
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
            double k = 1.0 / vertVec->at(i)->neighborIdx.size();
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

	Eigen::VectorXd bx(n);
	Eigen::VectorXd by(n);
	Eigen::VectorXd bz(n);
	bx*=0;
	by*=0;
	bz*=0;
	for(int i=0; i<vertVec->size(); i++){
		if(idx2row[i] != -1){
			double k = 1.0 / vertVec->at(i)->neighborIdx.size();
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

	Eigen::VectorXd x = solver.solve(bx);
	Eigen::VectorXd y = solver.solve(by);
	Eigen::VectorXd z = solver.solve(bz);

    for(int i=0; i<n; i++)
        vertVec->at(row2idx[i])->position() = point(x(i), y(i), z(i));

	printf("Boundary : %d, Inner: %d\n", vertVec->size() - n, n);
	END("Time of Formula");

    updateGL();
	
	SW(TOOL::UNDO, ACTIVATE);
	SW(TOOL::MINSURF_ITER, INACTIVATE);
	SW(TOOL::MINSURF_FORM, INACTIVATE);
}

void RenderingWidget::ClosedCurve(){
#ifdef ENABLE_UNDO
	cur_mesh_->WriteToOBJFile("tmp.obj");
	mesh_vec_.push_back(cur_mesh_);
	cur_mesh_ = new Mesh3D;
	cur_mesh_->LoadFromOBJFile("tmp.obj");
#endif
	START();

	std::vector<HE_vert*> * V = cur_mesh_->get_vertex_list();
	
	std::vector<QPointF> VonCircle;
	std::vector<point> VonCurve;
	float alpha = 0.0f;
	const float delta = 2 * PI / V->size();
	QPolygonF poly;
	for(int i=0; i < V->size(); i++, alpha += delta){
		poly.push_back(QPointF(cos(alpha), sin(alpha)));

		point SonCurve(V->at(i)->position());
		point TonCurve(V->at((i+1) % V->size())->position());
		point StoTonCurve = TonCurve - SonCurve;

		QPointF SonCircle(cos(alpha), sin(alpha));
		QPointF TonCircle(cos(alpha+delta), sin(alpha+delta));
		QPointF StoTonCircle = TonCircle - SonCircle;
		
		for(float k=0.0f, d = 1.0f/20.0f; k<1; k += d){
			VonCurve.push_back(SonCurve + k *  StoTonCurve);
			VonCircle.push_back(SonCircle + k *  StoTonCircle);
		}
	}
	srand( (unsigned)time( NULL ) );
	for(int cnt=0; cnt<64 * V->size(); ){
		float S = (rand()%100000)/100000.0;
		float r = sqrt(S/PI);
		float theta = (rand()%100000)/100000.0 * 2 * PI;
		float x = r * cos(theta);
		float y = r * sin(theta);
		if(poly.containsPoint(QPointF(x,y), Qt::OddEvenFill)){
			VonCircle.push_back(QPointF(x,y));
			cnt++;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	std::vector<std::vector<int>> faces;
	Triangulate(VonCircle, VonCircle.size(), faces, "cz");
	//---------------------------------------------------------------------------------------------------------
	FILE * rstF = fopen("tmp.obj", "w");
	for(int i=0; i<VonCircle.size(); i++){
		if(i<VonCurve.size())
			fprintf(rstF, "v %f %f %f\n", VonCurve[i].x(),  VonCurve[i].y(),  VonCurve[i].z());
		else
			fprintf(rstF, "v %f %f %f\n", VonCircle[i].x(), VonCircle[i].y(), 0.0f);
	}
	for(int i=0; i<faces.size(); i++)
		fprintf(rstF, "f %d %d %d\n", faces[i][0]+1, faces[i][1]+1, faces[i][2]+1);
	fclose(rstF);
	ReadMeshFrom(QString("tmp.obj"));
	//----------------------------------------------------------------------------------------------------
	Formula();
#ifdef ENABLE_UNDO
	mesh_vec_.back()->ClearData();
	mesh_vec_.pop_back();
#endif
	Formula();
#ifdef ENABLE_UNDO
	mesh_vec_.back()->ClearData();
	mesh_vec_.pop_back();
#endif
	updateGL();
	END("Time of Closed Curve");
	//----------------------------------------------------------------------------------------------------
	SW(TOOL::UNDO, ACTIVATE);
	SW(TOOL::MINSURF_CURVE, INACTIVATE);
}

void RenderingWidget::Undo(){
	if(mesh_vec_.size()>0){
		cur_mesh_->ClearData();
		SafeDelete(cur_mesh_);
		cur_mesh_ = mesh_vec_.back();
		mesh_vec_.pop_back();

		if(mesh_vec_.size()==0)
			SW(TOOL::UNDO, INACTIVATE);

		if(cur_mesh_->get_vertex_list()->size() == (cur_mesh_->get_edges_list()->size()/2)){
			SW(TOOL::MINSURF_CURVE, ACTIVATE);
			SW(TOOL::MINSURF_ITER, INACTIVATE);
			SW(TOOL::MINSURF_FORM, INACTIVATE);
		}else{
			SW(TOOL::MINSURF_CURVE, INACTIVATE);
			SW(TOOL::MINSURF_ITER, ACTIVATE);
			SW(TOOL::MINSURF_FORM, ACTIVATE);
		}

		updateGL();
	}
}

void RenderingWidget::Triangulate(std::vector<QPointF> & points,
	int segmentNum, std::vector<std::vector<int>> & faces, char * mode){
	struct triangulateio in, out;

	// points
	in.numberofpoints = points.size();
	in.pointlist = (TRI_REAL *) malloc(in.numberofpoints * 2 * sizeof(TRI_REAL));
	for(int i = 0; i < points.size(); i++){
		in.pointlist[2*i] = points[i].x();
		in.pointlist[2*i+1] = points[i].y();
	}

	// segments
	in.numberofsegments = segmentNum;
	in.segmentlist = (int *) malloc(segmentNum * 2 * sizeof(int));
	for(int i=0; i<segmentNum; i++){
		in.segmentlist[2*i] = i;
		in.segmentlist[2*i+1] = (i+1)%segmentNum;
	}

	//boundary marker
	in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
	for(int i=0; i<in.numberofpoints; i++)
		in.pointmarkerlist[i] = i<in.numberofsegments? 1 : 0;

	in.segmentmarkerlist = (int *) malloc(segmentNum * sizeof(int));
	for(int i=0; i<segmentNum; i++)
		in.segmentmarkerlist[i] = 1;
	
	in.numberofpointattributes = 0;
	in.pointattributelist = (TRI_REAL *) NULL;
	in.numberofholes = 0;
	in.numberofregions = 0;
	in.regionlist = (TRI_REAL *) NULL;
	out.trianglearealist = (TRI_REAL *) NULL;
    out.pointlist = (TRI_REAL *) NULL;
	out.pointattributelist = (TRI_REAL *) NULL;
	out.pointmarkerlist = (int *) NULL;
	out.trianglelist = (int *) NULL;
	out.triangleattributelist = (TRI_REAL *) NULL;
	out.neighborlist = (int *) NULL;
	out.segmentlist = (int *) NULL;
	out.segmentmarkerlist = (int *) NULL;
	out.edgelist = (int *) NULL;
	out.edgemarkerlist = (int *) NULL;

	triangulate(mode, &in, &out, (struct triangulateio *)NULL);

	// add new points
	for(int i = points.size(); i < out.numberofpoints; i++)
		points.push_back(QPointF(out.pointlist[i*2], out.pointlist[i*2+1]));

	// faces
	for(int i = 0; i< out.numberoftriangles; i++){
		faces.push_back(std::vector<int>());
		for(int j = 0; j < out.numberofcorners; j++)
			faces[i].push_back(out.trianglelist[i * out.numberofcorners + j]);
	}

	free(in.pointlist);
	free(in.pointattributelist);
	free(in.pointmarkerlist);
	free(in.regionlist);
	free(in.segmentmarkerlist);
	free(out.pointlist);
	free(out.pointattributelist);
	free(out.pointmarkerlist);
	free(out.trianglelist);
	free(out.triangleattributelist);
	free(out.trianglearealist);
	free(out.neighborlist);
	free(out.segmentlist);
	free(out.segmentmarkerlist);
	free(out.edgelist);
	free(out.edgemarkerlist);
}

FP RenderingWidget::method(int id){
	FP m[] = {&RenderingWidget::ReadMesh, &RenderingWidget::LoadTexture, &RenderingWidget::WriteMesh, &RenderingWidget::SetBackground, 
		&RenderingWidget::Iteration, &RenderingWidget::Formula, &RenderingWidget::ClosedCurve,
		&RenderingWidget::Undo};
	return m[id];
}