#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include <QGLWidget>
#include <QtCore/QEvent>

#include "HE_mesh/Vec.h"
#include "triangle.h"
#include "Param_FB_U.h"
#include "Param_FB_SP.h"
#include "Param_FB_WLS.h"
#include "Param_UFB_ARAP.h"

using trimesh::vec;
using trimesh::point;

class MainWindow;
class CArcBall;
class Mesh3D;

#define FILETOOLNUM 8
#define MINSURFTOOLNUM 4
#define TOOLNUM ((FILETOOLNUM)+(MINSURFTOOLNUM))

// 采用事件机制来控制 icon
#define SW(tool, status) (parent_->event(&QEvent(eventType[((tool)<<1)|(status)])))
// status
#define INACTIVATE 0
#define ACTIVATE 1

#define ENABLE_UNDO 

namespace TOOL{
	enum {
		OBJ,
		LOAD_TEX,
		TEX_U,
		TEX_WLS,
		TEX_SP,
		TEX_ARAP,
		SAVEAS,
		BG,
		MINSURF_ITER,
		MINSURF_FORM,
		MINSURF_CURVE,
		UNDO,
	};
}

// FP 是 函数指针
class RenderingWidget;//声明提前
typedef void (RenderingWidget::* FP)();

class RenderingWidget : public QGLWidget
{
	Q_OBJECT

public:
	RenderingWidget(QWidget *parent, QEvent::Type * _eventType, MainWindow* mainwindow=0);
	~RenderingWidget();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void timerEvent(QTimerEvent *e);

	// mouse events
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);

public:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	// 返回该类的函数指针
	static FP method(int id);

signals:
	void meshInfo(int, int, int);
	void operatorInfo(QString);

private:
	void Render();
	void SetLight();

	public slots:
	void SetBackground();
	bool ReadMeshFrom(QString & filename);
	void ReadMesh();
	void Load_Tex();
	void Tex_U();
	void Tex_WLS();
	void Tex_SP();
	void Tex_ARAP();
	void WriteMesh();

	// minimal surface
	void Iteration();
	void Formula();
	void ClosedCurve();
	void Undo();

	void CheckDrawPoint(bool bv);
	void CheckDrawEdge(bool bv);
	void CheckDrawFace(bool bv);
	void CheckLight(bool bv);
	void CheckDrawTexture(bool bv);
	void CheckDrawAxes(bool bv);

private:
	void DrawAxes(bool bv);
	void DrawPoints(bool);
	void DrawEdge(bool);
	void DrawFace(bool);
	void DrawTexture(bool);
	void Triangulate(std::vector<QPointF> & points, int segments,
		std::vector<std::vector<int>> & faces, char * mode);

public:
	MainWindow					*ptr_mainwindow_;
	CArcBall					*ptr_arcball_;
	Mesh3D						*cur_mesh_;
	std::vector<Mesh3D*>        mesh_vec_;

	// Texture
	GLuint						texture_[1];
	bool						is_load_texture_;

	// eye
	GLfloat						eye_distance_;
	point						eye_goal_;
	vec							eye_direction_;
	QPoint						current_position_;

	// Render information
	bool						is_draw_point_;
	bool						is_draw_edge_;
	bool						is_draw_face_;
	bool						is_draw_texture_;
	bool						has_lighting_;
	bool						is_draw_axes_;

private:
	// 自定义的 eventType
	// eventType[i] > QEvent::User, i = 0, 1, ..., 2*TOOLNUM-1
	QEvent::Type				eventType[2 * TOOLNUM];
	QObject						*parent_;
};

#endif // RENDERINGWIDGET_H
