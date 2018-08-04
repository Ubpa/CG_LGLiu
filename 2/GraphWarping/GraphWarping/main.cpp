#include "graphwarping.h"
#include <QtWidgets/QApplication>

#include <Eigen/Dense>
using namespace Eigen;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GraphWarping gw;
	gw.show();
	return a.exec();
}
