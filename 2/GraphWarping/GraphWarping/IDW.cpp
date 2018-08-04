#include "IDW.h"
#define U 2

IDW::IDW(std::vector<QPoint *> *_pVec, std::vector<QPoint *> *_qVec){
	u = U;

	if( _pVec->size() != _qVec->size() )
		return;
	for(int i=0; i < _pVec->size(); i++){
		pVec.push_back(new QPoint(*(_pVec->at(i))));
		qVec.push_back(new QPoint(*(_qVec->at(i))));
	}
}

IDW::IDW(std::vector<QPoint *> *_pVec, std::vector<QColor *> *_colorVec){
	u = U;

	if( _pVec->size() != _colorVec->size() )
		return;
	for(int i=0; i < _pVec->size(); i++){
		pVec.push_back(new QPoint(*(_pVec->at(i))));
		colorVec.push_back(new QColor(*(_colorVec->at(i))));
	}
}

IDW::~IDW(){
	for(int i=0; i < pVec.size(); i++)
		delete pVec[i];
	for(int i=0; i < qVec.size(); i++)
		delete qVec[i];
	for(int i=0; i < colorVec.size(); i++)
		delete colorVec[i];
}

QPoint IDW::map(QPoint p){
	std::vector<double> simVec;// 相似度向量
	double sumSim = calSim(&simVec, p);
	
	double qX = p.x(), qY = p.y();
	for(int i=0; i < pVec.size(); i++){
		double w = simVec[i] / sumSim;
		qX += w *  (qVec[i]->x() - pVec[i]->x());
		qY += w *  (qVec[i]->y() - pVec[i]->y());
	}
	
	return QPoint(qX, qY);
}

QColor IDW::mapColor(QPoint p){
	std::vector<double> simVec;
	double sumSim = calSim(&simVec, p);

	double r=0, g=0, b=0;
	for(int i=0; i < pVec.size(); i++){
		double w = simVec[i] / sumSim;
		r += w *  colorVec[i]->red();
		g += w *  colorVec[i]->green();
		b += w *  colorVec[i]->blue();
	}
	
	return QColor(r, g, b);
}

double IDW::calSim(std::vector<double> * simVec, QPoint p){
	double sumSim = 0, curSim;
	for(int i=0; i < pVec.size(); i++){
		QPoint diff = *pVec[i] - p;
		double d = sqrt(pow(diff.x(), 2.0) + pow(diff.y(), 2.0));
		curSim = 1.0 / pow(d, u);
		sumSim += curSim;
		simVec->push_back(curSim);
	}
	return sumSim;
}