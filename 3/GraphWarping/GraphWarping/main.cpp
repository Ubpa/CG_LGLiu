#include "graphwarping.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]){
	QApplication a(argc, argv);
	GraphWarping gw;
	gw.show();
	return a.exec();
}
