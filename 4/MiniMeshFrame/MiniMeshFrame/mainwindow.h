#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <string>
#include "ui_mainwindow.h"
#include "renderingwidget.h"

class QLabel;
class QPushButton;
class QCheckBox;
class QGroupBox;
class RenderingWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	// 用于从 renderingwidget 接收关于改变 icon 的信号
	bool event(QEvent * event);

private:
	void Init();
	void CreateActions();
	void CreateMenus();
	void CreateToolBars();
	void CreateStatusBar();
	void CreateRenderGroup();

protected:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	public slots:
	void ShowMeshInfo(int npoint, int nedge, int nface);

private:
	Ui::MainWindowClass ui;

	//---------------------------------------------------
	void							activate(int id);
	void							inactivate(int id);

	QMenu							*fileMenu, *minSurfMenu;
	QToolBar						*fileToolBar, *minSurfToolBar;
	std::vector<QAction *>			actions;
	std::string						toolName[TOOLNUM];
	// 自定义的 eventType
	// eventType[i] > QEvent::User, i = 0, 1, ..., 2*TOOLNUM-1
	QEvent::Type					eventType[2 * TOOLNUM];
	float								iconState[TOOLNUM];
	//---------------------------------------------------

	// Render RadioButtons
	QCheckBox						*checkbox_point_;
	QCheckBox						*checkbox_edge_;
	QCheckBox						*checkbox_face_;
	QCheckBox						*checkbox_light_;
	QCheckBox						*checkbox_texture_;
	QCheckBox						*checkbox_axes_;

	QGroupBox						*groupbox_render_;

	// Information
	QLabel							*label_meshinfo_;
	QLabel							*label_operatorinfo_;

	RenderingWidget					*renderingwidget_;
};

#endif // MAINWINDOW_H
