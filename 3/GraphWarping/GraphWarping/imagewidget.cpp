#include "imagewidget.h"

ImageWidget::ImageWidget(QEvent::Type * _eventType){
	curImg = new QImage();
	mode = IDLE;
	curLine = NULL;
	featureState = 0;
	setMouseTracking(true);
	for(int i=0; i<2*TOOLNUM; i++)
		eventType[i] = _eventType[i];
}

ImageWidget::~ImageWidget(){
	for(int i=0; i < lineVec.size(); i++)
		if(lineVec[i] != NULL)
			delete lineVec[i];
	for(int i=0; i < imageVec.size(); i++)
		if(imageVec[i] != NULL)
			delete imageVec[i];
}

FP ImageWidget::method(int id){
	FP m[] = {&ImageWidget::Open, &ImageWidget::Save, &ImageWidget::SaveAs, 
		&ImageWidget::Get, &ImageWidget::Feature, 
		&ImageWidget::WarpIDW, &ImageWidget::WarpRBF, 
		&ImageWidget::WarpRectMesh, &ImageWidget::WarpTriRandMesh, &ImageWidget::WarpTriFeatureMesh,
		&ImageWidget::Undo};
	return m[id];
}

void ImageWidget::mousePressEvent(QMouseEvent * mouseEvent){
	QPoint pos = mouseEvent->pos();
	if (mode == GET  && curImg->rect().contains(pos-imgIdx()) && Qt::LeftButton == mouseEvent->button()){
		mode = DRAW;
		curLine = new Line(pos, pos);
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent * mouseEvent){
	QPoint pos = mouseEvent->pos()-imgIdx();
	if(curImg->rect().contains(pos)){
		// 设置 status bar
		QColor color = QColor(curImg->pixel(pos));
		char buff[128];
		sprintf(buff,"x:%d, y:%d, (r, g, b):(%d, %d, %d)",
			pos.x(), pos.y(),
			color.red(), color.green(), color.blue());
		parent()->findChild<QStatusBar*>()->showMessage(buff);
	}else
		parent()->findChild<QStatusBar*>()->clearMessage();

	if(mode == DRAW)
		curLine->update(mouseEvent->pos());
}

void ImageWidget::mouseReleaseEvent(QMouseEvent * mouseEvent){
	if(mode == DRAW && Qt::LeftButton == mouseEvent->button()){
		mode = GET;
		lineVec.push_back(curLine);
		curLine = NULL;
	}

	//updateIcon();
}

void ImageWidget::paintEvent(QPaintEvent *paintevent){
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect( (width()-curImg->width())/2, (height()-curImg->height())/2, curImg->width(), curImg->height());
	painter.drawImage(rect, *curImg); 
	
	// Draw Line
	if(mode != IDLE){
		for(int i=0; i < lineVec.size(); i++)
			lineVec[i]->Draw(painter);
		if(curLine != NULL)
			curLine->Draw(painter);
	}

	// Draw keypoints
	if(featureState == 1){
		QPen pen;
		pen.setWidth(4);
		pen.setColor(QColor(0, 255, 0));
		painter.setPen(pen);
		for(int i=0; i<keypoints.size(); i++)
			painter.drawPoint(QPoint(keypoints[i].pt.x, keypoints[i].pt.y) + imgIdx());
	}
	
	painter.end();

	updateIcon();
	update();
}

void ImageWidget::Open(){
	// Open file
	filename = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if(!filename.isEmpty()){
		curImg->load(filename);

		for(int i=0; i<imageVec.size(); i++)
			delete imageVec[i];
		imageVec.clear();
		
		printf("Image[ %s ] : %d x %d\n", filename.toStdString().c_str(), curImg->width(), curImg->height());

		

		// 清空 keypoints
		keypoints.clear();
		featureState = 0;
	}

	update();
}

void ImageWidget::Save(){
	curImg->save(filename);
}

void ImageWidget::SaveAs(){
	filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
		return;
	Save();
}

void ImageWidget::Get(){
	mode = GET;
}

void ImageWidget::Feature(){
	if(keypoints.size() == 0){
		cv::Mat imgMat = TypeCvt::QImageToMat(*curImg);
		cv::SurfFeatureDetector surf;
		surf.detect(imgMat, keypoints);
		featureState = 1;
	}else
		featureState ^= 1;
}

void ImageWidget::lineVecTo(std::vector<ND_Point> & _pVec, std::vector<ND_Point> & _fpVec){
	for(int i=0; i < lineVec.size(); i++){
		_pVec.push_back(lineVec[i]->getStartPoint() - imgIdx());
		_fpVec.push_back(lineVec[i]->getEndPoint() - lineVec[i]->getStartPoint());
		delete lineVec[i];
	}
	lineVec.clear();
}

void ImageWidget::WarpIDW(){
	START();

	printf("[ IDW ]\n");
	std::vector<ND_Point> p, fp;
	lineVecTo(p, fp);
	IDW idw(p, fp);

	// flag : 用 记录新图像的绘制情况
	//		  flag(i,j) == 0 -> (i,j) 未涂色
	//		  flag(i,j) == 1 -> (i,j) 已涂色
	MatrixXi flag(curImg->width(), curImg->height());
	flag *= 0;
	
	QImage * newImg = idw.warp(curImg, allImgP(), &flag);

	fix(newImg, flag);
	
	imageVec.push_back(curImg);
	curImg = newImg;
	mode = IDLE;

	// 清空 keypoints
	keypoints.clear();
	featureState = 0;
	
	//updateIcon();
	END("All Time");
}

void ImageWidget::WarpRBF(){
	START();

	printf("[ RBF ]\n");
	std::vector<ND_Point> p, fp;
	lineVecTo(p, fp);
	RBF rbf(p, fp);

	// flag : 用 记录新图像的绘制情况
	//		  flag(i,j) == 0 -> (i,j) 未涂色
	//		  flag(i,j) == 1 -> (i,j) 已涂色
	MatrixXi flag(curImg->width(), curImg->height());
	flag *= 0;

	QImage * newImg = rbf.warp(curImg, allImgP(), &flag);

	fix(newImg, flag);

	imageVec.push_back(curImg);
	curImg = newImg;
	mode = IDLE;

	// 清空 keypoints
	keypoints.clear();
	featureState = 0;
	
	//updateIcon();
	END("All Time");
}

void ImageWidget::WarpRectMesh(){
	START();

	printf("[ Rectangle Mesh ]\n");
	std::vector<ND_Point> p, fp;
	lineVecTo(p, fp);
	IDW idw(p, fp);

	Mesh mesh;
	mesh.loadRectMesh(curImg, 10);
	
	QImage * newImg = mesh.warp(curImg, idw);

	imageVec.push_back(curImg);
	curImg = newImg;
	mode = IDLE;

	// 清空 keypoints
	keypoints.clear();
	featureState = 0;
	
	//updateIcon();
	END("All Time");
}

void ImageWidget::WarpTriRandMesh(){
	START();

	printf("[ Triangle Random Mesh ]\n");
	std::vector<ND_Point> p, fp;
	lineVecTo(p, fp);
	IDW idw(p, fp);

	Mesh mesh;
	mesh.loadTriRandMesh(curImg, curImg->width()*curImg->height()/400);
	
	QImage * newImg = mesh.warp(curImg, idw);

	imageVec.push_back(curImg);
	curImg = newImg;
	mode = IDLE;

	// 清空 keypoints
	keypoints.clear();
	featureState = 0;
	
	//updateIcon();
	END("All Time");
}

void ImageWidget::WarpTriFeatureMesh(){
	START();
	printf("[ Triangel Feature Mesh ]\n");
	std::vector<ND_Point> p, fp;
	lineVecTo(p, fp);
	IDW idw(p, fp);

	Mesh mesh;
	//mesh.loadRectMesh(curImg, 10);
	mesh.loadTriFeatureMesh(curImg);
	
	QImage * newImg = mesh.warp(curImg, idw);

	imageVec.push_back(curImg);
	curImg = newImg;
	mode = IDLE;

	// 清空 keypoints
	keypoints.clear();
	featureState = 0;
	
	//updateIcon();
	END("All Time");
}

void ImageWidget::fix(QImage * img, MatrixXi & flag){
	START();

	for(int i=0; i<img->width(); i++){
		for( int j=0; j<img->height(); j++){
			if( flag(i, j) == 0 ){
				// find neighbor
				std::vector<ND_Point> neighborVec;
				std::vector<ND_Point> colorVec;
				for(int x = i-2; x<=i+2; x++){
					for(int y = j-2; y<=j+2; y++){
						if( img->rect().contains(QPoint(x, y)) && flag(x, y) != 0 ){
							neighborVec.push_back(ND_Point(x,y));
							colorVec.push_back(ND_Point(QColor(img->pixel(x,y))));
						}
					}
				}
				if(neighborVec.size()==0)
					continue;
				
				// interpolation with neighbor
				IDW idw(neighborVec, colorVec);
				QRgb newColor = QColor(idw.map(ND_Point(i, j))).rgb();
				img->setPixel(i, j, newColor);
			}
		}
	}
	
	END("Time of fixing");
}

void ImageWidget::Undo(){
	if(mode == IDLE && imageVec.size() > 0){
		delete curImg;
		curImg = imageVec.back();
		imageVec.pop_back();
	}

	if(mode == GET && lineVec.size() > 0){
		delete lineVec.back();
		lineVec.pop_back();
	}
	
	
	//updateIcon();
	update();
}

QPoint ImageWidget::imgIdx(){
	if(curImg != NULL){
		int diffX = (width()-curImg->width())/2;
		int diffY = (height()-curImg->height())/2;
		return QPoint(diffX, diffY);
	}else
		return QPoint(-1, -1);
}

std::vector<ND_Point> ImageWidget::allImgP(){
	std::vector<ND_Point> points;
	for(int i=0; i<curImg->width(); i++)
		for(int j=0; j<curImg->height(); j++)
			points.push_back(ND_Point(i,j));
	return points;
}
 
void ImageWidget::updateIcon(){
	SW(TOOL::OPEN, ACTIVATE);
	SW(TOOL::SAVE, filename.isEmpty()?0:1);
	SW(TOOL::SAVEAS, filename.isEmpty()?0:1);
	SW(TOOL::GET, filename.isEmpty()?0:1);
	SW(TOOL::FEATURE, filename.isEmpty()?0:1);
	SW(TOOL::IDW, lineVec.size()>0?1:0);
	SW(TOOL::RBF, lineVec.size()>0?1:0);
	SW(TOOL::RECT_MESH, lineVec.size()>0?1:0);
	SW(TOOL::TRI_RAND_MESH, lineVec.size()>0?1:0);
	SW(TOOL::TRI_FEATURE_MESH, lineVec.size()>0?1:0);
	SW(TOOL::UNDO, (lineVec.size()>0 || imageVec.size()>0)?1:0);
}