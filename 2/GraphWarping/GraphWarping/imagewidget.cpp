#include "imagewidget.h"
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

ImageWidget::ImageWidget(){
	currentImage = new QImage();
	mode = IDLE;
	currentLine = NULL;
	setMouseTracking(true);
}

ImageWidget::~ImageWidget()
{
	for(int i=0; i < lineVec.size(); i++)
		if(lineVec[i] != NULL)
			delete lineVec[i];
	for(int i=0; i < imageVec.size(); i++)
		if(imageVec[i] != NULL)
			delete imageVec[i];
}

void ImageWidget::mousePressEvent(QMouseEvent * mouseEvent){
	QPoint position = mouseEvent->pos();
	if (insideImg(position) && mode == GET && Qt::LeftButton == mouseEvent->button()){
		mode = DRAW;
		currentLine = new Line(position, position);
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent * mouseEvent){
	QPoint position = mouseEvent->pos();
	if(insideImg(position)){
		// 获取相对于图像的坐标 position
		int diffX = (width()-currentImage->width())/2;
		int diffY = (height()-currentImage->height())/2;
		QPoint diff = QPoint(diffX, diffY);
		position -= diff;
		
		// 设置 status bar
		QColor color = QColor(currentImage->pixel(position));
		char buff[128];
		sprintf(buff,"x:%d, y:%d, (r, g, b):(%d, %d, %d)",
			position.x(), position.y(),
			color.red(), color.green(), color.blue());
		parent()->findChild<QStatusBar*>()->showMessage(buff);
	}else
		parent()->findChild<QStatusBar*>()->clearMessage();

	if(mode == DRAW)
		currentLine->update(mouseEvent->pos());
}

void ImageWidget::mouseReleaseEvent(QMouseEvent * mouseEvent){
	if(mode == DRAW && Qt::LeftButton == mouseEvent->button()){
		mode = GET;
		lineVec.push_back(currentLine);
		currentLine = NULL;
	}
}

void ImageWidget::paintEvent(QPaintEvent *paintevent){
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect( (width()-currentImage->width())/2, (height()-currentImage->height())/2, currentImage->width(), currentImage->height());
	painter.drawImage(rect, *currentImage); 
	
	// Draw Line
	if(mode != IDLE){
		QPen pen;
		pen.setWidth(4);
		pen.setColor(QColor(255, 0, 0));
		painter.setPen(pen);
		for(int i=0; i < lineVec.size(); i++)
			lineVec[i]->Draw(painter);
		if(currentLine != NULL)
		currentLine->Draw(painter);
	}
	
	painter.end();
	update();
}

void ImageWidget::Open()
{
	// Open file
	filename = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!filename.isEmpty())
	{
		currentImage->load(filename);

		for(int i=0; i<imageVec.size(); i++)
			delete imageVec[i];
		imageVec.clear();
	}

	std::cout<<"image size: "<<currentImage->width()<<' '<<currentImage->height()<<std::endl;
	update();

}

void ImageWidget::Save(){
	currentImage->save(filename);
}

void ImageWidget::SaveAs()
{
	filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
		return;
	Save();
}

void ImageWidget::Get(){
	mode = mode == GET? IDLE : GET;
}

void ImageWidget::lineVecTo(std::vector<QPoint *> *p, std::vector<QPoint *> *q){
	int diffX = (width()-currentImage->width())/2;
	int diffY = (height()-currentImage->height())/2;
	QPoint diff = QPoint(diffX, diffY);

	for(int i=0; i < lineVec.size(); i++){
		p->push_back(new QPoint(lineVec[i]->getStartPoint()-diff));
		q->push_back(new QPoint(lineVec[i]->getEndPoint()-diff));
		delete lineVec[i];
	}
	lineVec.clear();
}

void ImageWidget::WarpIDW(){
	std::vector<QPoint *> *p = new std::vector<QPoint *>();
	std::vector<QPoint *> *q = new std::vector<QPoint *>();
	lineVecTo(p, q);

	IDW * idw = new IDW(p, q);
	warp(idw);
	mode = IDLE;
}

void ImageWidget::WarpRBF(){
	std::vector<QPoint *> *p = new std::vector<QPoint *>();
	std::vector<QPoint *> *q = new std::vector<QPoint *>();
	lineVecTo(p, q);

	RBF * rbf = new RBF(p, q);
	warp(rbf);
	mode = IDLE;
}

void ImageWidget::warp(Interpolation * interpolation){
	imageVec.push_back(currentImage);

	// flag : 用于记录新图像的绘制情况
	//		  flag(i,j) == 0 -> (i,j)空白
	//		  flag(i,j) == 1 -> (i,j)非空白
	MatrixXd flag(currentImage->width(), currentImage->height());
	for(int i=0; i<currentImage->width(); i++)
		for( int j=0; j<currentImage->height(); j++)
			flag(i,j) = 0;

	// 图像变换
	QImage * newImage = new QImage(currentImage->width(), currentImage->height(), QImage::Format_RGB888);
	for(int i=0; i < currentImage->width(); i++){
		for(int j=0; j < currentImage->height(); j++){
			QPoint p(i, j);
			QPoint q = interpolation->map(p);
			if( q.x() < currentImage->width() && q.x() >= 0
			&& q.y() < currentImage->height() && q.y() >= 0 ){
				flag(q.x(),q.y()) = 1;
				newImage->setPixel(q, currentImage->pixel(p));
			}
		}
	}

	// 填补空白
	fix(newImage, &flag);

	currentImage = newImage;
}

void ImageWidget::fix(QImage * newImage, MatrixXd *flag){
	for(int i=0; i<newImage->width(); i++){
		for( int j=0; j<newImage->height(); j++){
			if( (*flag)(i,j) == 0 ){
				std::vector<QPoint *> neiborVec;
				std::vector<QColor *> colorVec;
				for(int x = MAX(i-2,0); x<MIN(i+3,newImage->width()); x++){
					for(int y = MAX(j-2,0); y<MIN(j+3,newImage->height()); y++){
						if( (*flag)(x,y) != 0 ){
							neiborVec.push_back(new QPoint(x,y));
							colorVec.push_back(new QColor(newImage->pixel(x,y)));
						}
					}
				}
				IDW idw(&neiborVec, &colorVec);
				QRgb newColor = idw.mapColor(QPoint(i,j)).rgb();
				newImage->setPixel(i, j, newColor);
				for(int n=0; n<neiborVec.size(); n++)
					delete neiborVec[n], colorVec[n];
			}
		}
	}
}

void ImageWidget::Undo(){
	if(mode == IDLE && imageVec.size() > 0){
		delete currentImage;
		currentImage = imageVec.back();
		imageVec.pop_back();
	}
	if(mode == GET && lineVec.size() > 0){
		delete lineVec.back();
		lineVec.pop_back();
	}
	update();
}

bool ImageWidget::insideImg(QPoint p){
	if( currentImage == NULL )
		return false;

	int diffX = (width()-currentImage->width())/2;
	int diffY = (height()-currentImage->height())/2;
	QPoint diff = QPoint(diffX, diffY);
	QPoint position = p-diff;
	
	if( position.x() < currentImage->width() && position.x() >= 0
		&& position.y() < currentImage->height() && position.y() >= 0 )
		return true;
	else
		return false;
}