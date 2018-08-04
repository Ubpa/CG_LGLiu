#ifndef GRAPHWARPING_H
#define GRAPHWARPING_H

#include "ui_graphwarping.h"
#include <QtWidgets\QMainWindow>
#include <QtWidgets\qaction.h>
#include <QtWidgets\qmenu.h>
#include <QtWidgets\qmenubar.h>
#include <QtWidgets\qtoolbar.h>
#include <qline.h>
#include "imagewidget.h"
#include <vector>

class GraphWarping : public QMainWindow{
	Q_OBJECT

public:
	GraphWarping(QWidget *parent = 0);
	~GraphWarping();

private:
	void init();
	
	Ui::GraphWarpingClass ui;
	QMenu *fileMenu, *warpMenu;
	QToolBar *fileToolBar, *warpToolBar;
	std::vector<QAction *> fileActionVec, warpActionVec;

	ImageWidget		*imagewidget;
};

#endif // GRAPHWARPING_H
