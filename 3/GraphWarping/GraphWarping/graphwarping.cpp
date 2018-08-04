#include "graphwarping.h"

GraphWarping::GraphWarping(QWidget *parent)
: QMainWindow(parent){
	ui.setupUi(this);
	init();
}

GraphWarping::~GraphWarping(){
	for(int i = 0; i<TOOLNUM; i++)
		delete actions[i];
}

void GraphWarping::init(){
	// init toolName, iconState, eventType
	char * tn[TOOLNUM] = {"open", "save", "saveas",
	"get", "feature", "idw", "rbf", "rect_mesh", "tri_rand_mesh", "tri_feature_mesh", "undo"};
	for(int i=0; i<TOOLNUM; i++){
		toolName[i] = std::string(tn[i]);
		iconState[i] = -1;
		eventType[(i<<1)+0] = (QEvent::Type)QEvent::registerEventType();
		eventType[(i<<1)+1] = (QEvent::Type)QEvent::registerEventType();
	}

	// menu
	fileMenu = menuBar()->addMenu("File");
	warpMenu = menuBar()->addMenu("Warp");

	// tool bar
	fileToolBar = findChild<QToolBar*>();
	fileToolBar->setWindowTitle("File Tool");
	warpToolBar = addToolBar("Warp Tool");
	
	// action
	for(int i = 0; i < FILETOOLNUM; i++){
		actions.push_back(new QAction(QIcon(),toolName[i].c_str(), this));
		actions[i]->setStatusTip(toolName[i].c_str());
		fileToolBar->addAction(actions[i]);
		fileMenu->addAction(actions[i]);
	}
	for(int i = FILETOOLNUM; i < FILETOOLNUM + WARPTOOLNUM; i++){
		actions.push_back(new QAction(QIcon(),toolName[i].c_str(), this));
		actions[i]->setStatusTip(toolName[i].c_str());
		warpToolBar->addAction(actions[i]);
		warpMenu->addAction(actions[i]);
	}

	// image widget
	imagewidget = new ImageWidget(eventType);
    setCentralWidget(imagewidget);

	// activate / inactivate tool
	activate(TOOL::OPEN);// TOOL::OPEN == 0
	for(int i=TOOL::SAVE; i<=TOOL::UNDO; i++)
		inactivate(i);

}

void GraphWarping::activate(int id){
	if(iconState[id]!=1){
		actions[id]->setIcon(QIcon((PREFIX+toolName[id]+std::string(".bmp")).c_str()));
		connect(actions[id], &QAction::triggered, imagewidget, ImageWidget::method(id));
		iconState[id]=1;
	}
}

void GraphWarping::inactivate(int id){
	if(iconState[id]!=0){
		actions[id]->setIcon(QIcon((PREFIX+toolName[id]+std::string("_ban.bmp")).c_str()));
		disconnect(actions[id], &QAction::triggered, imagewidget, ImageWidget::method(id));
		iconState[id]=0;
	}
}

bool GraphWarping::event(QEvent * event){
	if(event->type() < QEvent::User)
		// not user event
		return QMainWindow::event(event);
	else{
		int i;
		for(i=1; i<2*TOOLNUM+1; i++){
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