#include "graphwarping.h"
#include <string>

#define FILETOOLNUM 3
#define WARPTOOLNUM 4

GraphWarping::GraphWarping(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	init();
}

GraphWarping::~GraphWarping()
{
	for(int i = 0; i<FILETOOLNUM; i++)
		delete fileActionVec[i];
	for(int i = 0; i<WARPTOOLNUM; i++)
		delete warpActionVec[i];
}

void GraphWarping::init(){
	// menu
	fileMenu = menuBar()->addMenu("File");
	warpMenu = menuBar()->addMenu("Warp");

	// tool bar
	fileToolBar = findChild<QToolBar*>();
	warpToolBar = addToolBar("Warp");
	
	// tool button
	std::string fileToolName[FILETOOLNUM] = {"open", "save", "saveas"};
	std::string warpToolName[WARPTOOLNUM] = {"get", "idw", "rbf", "undo"};
	for(int i=0; i<FILETOOLNUM; i++){
		fileActionVec.push_back(new QAction(
			QIcon((std::string(":/GraphWarping/Resources/images/")+fileToolName[i]+std::string(".bmp")).c_str()),
			fileToolName[i].c_str(), this));
		fileActionVec[i]->setStatusTip(fileToolName[i].c_str());
		fileToolBar->addAction(fileActionVec[i]);
		fileMenu->addAction(fileActionVec[i]);
	}
	for(int i=0; i<WARPTOOLNUM; i++){
		warpActionVec.push_back(new QAction(
			QIcon((std::string(":/GraphWarping/Resources/images/")+warpToolName[i]+std::string(".bmp")).c_str()),
			warpToolName[i].c_str(), this));
		warpActionVec[i]->setStatusTip(warpToolName[i].c_str());
		warpToolBar->addAction(warpActionVec[i]);
		warpMenu->addAction(warpActionVec[i]);
	}

	// signal slot
	imagewidget = new ImageWidget();
    setCentralWidget(imagewidget);
    connect(fileActionVec[0], &QAction::triggered, imagewidget, &ImageWidget::Open);
    connect(fileActionVec[1], &QAction::triggered, imagewidget, &ImageWidget::Save);
    connect(fileActionVec[2], &QAction::triggered, imagewidget, &ImageWidget::SaveAs);

	connect(warpActionVec[0], &QAction::triggered, imagewidget, &ImageWidget::Get);
	connect(warpActionVec[1], &QAction::triggered, imagewidget, &ImageWidget::WarpIDW);
	connect(warpActionVec[2], &QAction::triggered, imagewidget, &ImageWidget::WarpRBF);
	connect(warpActionVec[3], &QAction::triggered, imagewidget, &ImageWidget::Undo);
}