#include "mainwindow.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QMessageBox>
#include <QtGui\QKeyEvent>
#include "renderingwidget.h"

#define PREFIX (std::string(":/MainWindow/Resources/images/"))

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);


	setGeometry(300, 150, 800, 600);

	
	Init();
	renderingwidget_ = new RenderingWidget(this, eventType);
	//setCentralWidget(renderingwidget_);
	CreateActions();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();
	CreateRenderGroup();

	activate(TOOL::OBJ);
	activate(TOOL::BG);
	inactivate(TOOL::SAVEAS);
	inactivate(TOOL::TEX);
	inactivate(TOOL::MINSURF_ITER);
	inactivate(TOOL::MINSURF_FORM);
	inactivate(TOOL::MINSURF_CURVE);
	inactivate(TOOL::UNDO);

	QVBoxLayout *layout_left = new QVBoxLayout;
	layout_left->addWidget(groupbox_render_);
	layout_left->addStretch(4);

	QHBoxLayout *layout_main = new QHBoxLayout;

	layout_main->addLayout(layout_left);
	layout_main->addWidget(renderingwidget_);
	layout_main->setStretch(1, 1);
	this->centralWidget()->setLayout(layout_main);
}

MainWindow::~MainWindow()
{
	for(int i=0; i<actions.size();i++)
		delete actions[i];
}

void MainWindow::Init(){
	char tn[TOOLNUM][128] = {"load_obj", "load_tex", "saveas", "background", 
	"minSurf_Iter", "minSurf_Form", "minSurf_Curve", "undo"};
	for(int i=0; i<TOOLNUM; i++){
		toolName[i] = std::string(tn[i]);
		iconState[i] = -1;
		eventType[(i<<1)+0] = (QEvent::Type)QEvent::registerEventType();
		eventType[(i<<1)+1] = (QEvent::Type)QEvent::registerEventType();
	}
}


void MainWindow::CreateActions()
{
	// action
	for(int i = 0; i < FILETOOLNUM; i++){
		actions.push_back(new QAction(QIcon(),toolName[i].c_str(), this));
		actions[i]->setStatusTip(toolName[i].c_str());
	}
	for(int i = FILETOOLNUM; i < FILETOOLNUM + MINSURFTOOLNUM; i++){
		actions.push_back(new QAction(QIcon(),toolName[i].c_str(), this));
		actions[i]->setStatusTip(toolName[i].c_str());
	}
}

void MainWindow::CreateMenus()   {
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->setStatusTip(tr("File Menu"));
	for(int i=0; i<FILETOOLNUM; i++)
		fileMenu->addAction(actions[i]);

	minSurfMenu = menuBar()->addMenu(tr("&Min Surf"));
	minSurfMenu->setStatusTip(tr("Minimal Surface Menu"));
	for(int i=FILETOOLNUM; i<FILETOOLNUM+MINSURFTOOLNUM; i++)
		minSurfMenu->addAction(actions[i]);
}

void MainWindow::CreateToolBars(){
	fileToolBar = findChild<QToolBar*>();
	fileToolBar->setWindowTitle(tr("&File Tool"));
	for(int i=0; i<FILETOOLNUM; i++)
		fileToolBar->addAction(actions[i]);

	minSurfToolBar = addToolBar(tr("&Minimal Surface Tool"));
	for(int i=FILETOOLNUM; i<FILETOOLNUM+MINSURFTOOLNUM; i++)
		minSurfToolBar->addAction(actions[i]);
}

void MainWindow::CreateStatusBar(){
	label_meshinfo_ = new QLabel(QString("MeshInfo: p: %1 e: %2 f: %3").arg(0).arg(0).arg(0));
	label_meshinfo_->setAlignment(Qt::AlignCenter);
	label_meshinfo_->setMinimumSize(label_meshinfo_->sizeHint());

	label_operatorinfo_ = new QLabel();
	label_operatorinfo_->setAlignment(Qt::AlignVCenter);
	

	statusBar()->addWidget(label_meshinfo_);
	connect(renderingwidget_, SIGNAL(meshInfo(int, int, int)), this, SLOT(ShowMeshInfo(int, int, int)));

	statusBar()->addWidget(label_operatorinfo_);
	connect(renderingwidget_, SIGNAL(operatorInfo(QString)), label_operatorinfo_, SLOT(setText(QString)));
}

void MainWindow::CreateRenderGroup()
{
	checkbox_point_ = new QCheckBox(tr("point"), this);
	connect(checkbox_point_, SIGNAL(clicked(bool)), renderingwidget_, SLOT(CheckDrawPoint(bool)));
	checkbox_point_->setChecked(true);

	checkbox_edge_ = new QCheckBox(tr("Edge"), this);
	connect(checkbox_edge_, SIGNAL(clicked(bool)), renderingwidget_, SLOT(CheckDrawEdge(bool)));
	checkbox_edge_->setChecked(true);

	checkbox_face_ = new QCheckBox(tr("Face"), this);
	connect(checkbox_face_, SIGNAL(clicked(bool)), renderingwidget_, SLOT(CheckDrawFace(bool)));
	
	checkbox_light_ = new QCheckBox(tr("Light"), this);
	connect(checkbox_light_, SIGNAL(clicked(bool)), renderingwidget_, SLOT(CheckLight(bool)));

	checkbox_texture_ = new QCheckBox(tr("Texture"), this);
	connect(checkbox_texture_, SIGNAL(clicked(bool)), renderingwidget_, SLOT(CheckDrawTexture(bool)));

	checkbox_axes_ = new QCheckBox(tr("Axes"), this);
	connect(checkbox_axes_, SIGNAL(clicked(bool)), renderingwidget_, SLOT(CheckDrawAxes(bool)));

	groupbox_render_ = new QGroupBox(tr("Render"), this);

	QVBoxLayout* render_layout = new QVBoxLayout(groupbox_render_);
	render_layout->addWidget(checkbox_point_);
	render_layout->addWidget(checkbox_edge_);
	render_layout->addWidget(checkbox_face_);
	render_layout->addWidget(checkbox_texture_);
	render_layout->addWidget(checkbox_light_);
	render_layout->addWidget(checkbox_axes_);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{

}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{

}

void MainWindow::ShowMeshInfo(int npoint, int nedge, int nface)
{
	label_meshinfo_->setText(QString("MeshInfo: p: %1 e: %2 f: %3").arg(npoint).arg(nedge).arg(nface));
}


void MainWindow::activate(int id){
	if(iconState[id]!=1){
		actions[id]->setIcon(QIcon((PREFIX+toolName[id]+std::string(".bmp")).c_str()));
		connect(actions[id], &QAction::triggered, renderingwidget_, RenderingWidget::method(id));
		iconState[id]=1;
	}
}

void MainWindow::inactivate(int id){
	if(iconState[id]!=0){
		actions[id]->setIcon(QIcon((PREFIX+toolName[id]+std::string("_ban.bmp")).c_str()));
		disconnect(actions[id], &QAction::triggered, renderingwidget_, RenderingWidget::method(id));
		iconState[id]=0;
	}
}

bool MainWindow::event(QEvent * event){
	if(event->type() < QEvent::User)
		// not user event
		return QMainWindow::event(event);
	else{
		int i;
		for(i=0; i<2*TOOLNUM; i++){
			if(eventType[i] == event->type())
				break;
		}
		// i = (id << 1) | (mode)
		bool mode = i & 0x1;
		unsigned int id = (i>>1) & 0xF;
		if(mode)
			activate(id);
		else
			inactivate(id);
		return true;
	}
}