#include "viewwidget.h"
#include <QMouseEvent>
#include <qpainter.h>
#include "qcolordialog.h"
#include "qinputdialog.h"

ViewWidget::ViewWidget(QWidget *parent)
    : QWidget(parent){
    ui.setupUi(this);
    draw_status = false;
    current_figure = NULL;
    figure_type = kDefault;
	pointHovered = nullptr;
	canDrag = true;
	setMouseTracking(true);
	linePen.setWidth(5);
}

ViewWidget::~ViewWidget(){
    for(int i=0; i<figureVec.size(); i++){
        if(figureVec[i]){
            delete figureVec[i];
            figureVec[i] = NULL;
        }
    }

    // 每次把 current_figure push to figureVec 后都将其设置为 NULL
    // 因此下面的 delete 和上面的 delete 不会处理同一指针
    if(current_figure != NULL)
        delete current_figure;
}

void ViewWidget::mousePressEvent(QMouseEvent *mouseEvent){
	if(figure_type == kDrag)
		return;

	QPoint pos = mouseEvent->pos();

	// drag point
	if(draw_status == false && figure_type != kDrag){
		bool mode = Qt::LeftButton == mouseEvent->button() ? false : true;
		for(int i=0; i<figureVec.size(); i++){
			dragPoints = figureVec[i]->getPoint(pos, mode);
			if(dragPoints.size() != 0){
				ft_backup = figure_type;
				figure_type = kDrag;
				return;
			}
		}
	}

	if(Qt::LeftButton == mouseEvent->button()){
		// start draw

		draw_status = true;
        switch (figure_type){
	    case kDefault:
            // 处于初始状态
            draw_status = false;
		    break;
	    case kLine:
		    current_figure = new Line(pos, pos);
			current_figure->setCanDrag(canDrag);
			current_figure->setPen(&linePen, PEN::LINE);
		    break;
        case kRectangle:
		    current_figure = new Rect(pos, pos);
			current_figure->setCanDrag(canDrag);
			current_figure->setPen(&linePen, PEN::LINE);
		    break;
	    case kEllipse:
		    current_figure = new Ellipse(pos, pos);
			current_figure->setCanDrag(canDrag);
			current_figure->setPen(&linePen, PEN::LINE);
		    break;
        case kPolygon:
            if( current_figure == NULL ){
		        current_figure = new Polygon(pos);
				current_figure->setCanDrag(canDrag);
				current_figure->setPen(&linePen, PEN::LINE);
                // 保持鼠标追踪
                // 无论鼠标是否有键按下，都会持续触发 mouseMoveEvent
                setMouseTracking(true);
            }
            break;
        case kFreehand:
            current_figure = new Freehand(pos);
			current_figure->setCanDrag(canDrag);
			current_figure->setPen(&linePen, PEN::LINE);
            break;
		case kBezierCurve:
			if( current_figure == NULL ){
		        current_figure = new BezierCurve(pos);
				current_figure->setCanDrag(canDrag);
				current_figure->setPen(&linePen, PEN::LINE);
                // 保持鼠标追踪
                // 无论鼠标是否有键按下，都会持续触发 mouseMoveEvent
                setMouseTracking(true);
            }
			break;
	    default:
		    break;
	    }
	}
}

void ViewWidget::mouseMoveEvent(QMouseEvent *mouseEvent){
	end_point = mouseEvent->pos();
	if (draw_status){
		current_figure->update(end_point);
    }else{
		// update drag point
		if(figure_type==kDrag){
			QPoint diff = end_point - *dragPoints[0];
			for(int i=0; i<dragPoints.size(); i++){
				dragPoints[i]->setX(dragPoints[i]->x()+diff.x());
				dragPoints[i]->setY(dragPoints[i]->y()+diff.y());
			}
		}

		// delete circle around hovered point
		if(pointHovered){
			delete figureVec.back();
			figureVec.pop_back();
		}

		// draw circle around hovered point
		QPoint pos = mouseEvent->pos();
		for(int i=0; i<figureVec.size(); i++){
			std::vector<QPoint *> point = figureVec[i]->getPoint(pos, false);
			pointHovered = point.size()==1 ? point[0] : nullptr;
			if(pointHovered != nullptr){
				figureVec.push_back(new Ellipse(pos-QPoint(20,20), pos+QPoint(20,20)));
				QPen circlePen;
				circlePen.setWidth(2);
				circlePen.setColor(QColor(0x22, 0xB1, 0x4C));
				figureVec.back()->setPen(&circlePen, PEN::LINE);
				figureVec.back()->setCanDrag(false);
				break;
			}
		}
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent *mouseEvent){	
    if(draw_status == true && (figure_type == kPolygon || figure_type == kBezierCurve)){
		if(Qt::LeftButton == mouseEvent->button())
            current_figure->update(1);// 新增一个点
        if(Qt::RightButton == mouseEvent->button()){
            draw_status = false;
            current_figure->update(0);// 绘制完成
            figureVec.push_back(current_figure);
            current_figure = NULL;
            setMouseTracking(canDrag);
        }
	}else if(figure_type == kDrag){
		dragPoints.clear();
		figure_type = ft_backup;
		setMouseTracking(canDrag);
	}else{
        draw_status = false;
        if(current_figure != NULL){
            figureVec.push_back(current_figure);
	        current_figure = NULL;
        }
    }
}

void ViewWidget::paintEvent(QPaintEvent *)
{
	QPainter painter;
	painter.begin(this);
	painter.setPen(linePen);

    for(int i=0; i < figureVec.size(); i++)
        figureVec[i]->Draw(painter);
    if(current_figure != NULL)
        current_figure->Draw(painter);

	painter.end();
	update();
}

void ViewWidget::set_figure_type_to_line(){
	if(draw_status == false)
        figure_type = kLine;
}

void ViewWidget::set_figure_type_to_ellipse(){
    if(draw_status == false)
	    figure_type = kEllipse;
}

void ViewWidget::set_figure_type_to_rectangle(){
	if(draw_status == false)
        figure_type = kRectangle;
}

void ViewWidget::set_figure_type_to_polygon(){
	if(draw_status == false)
        figure_type = kPolygon;
}

void ViewWidget::set_figure_type_to_freehand(){
    if(draw_status == false)
        figure_type = kFreehand;
}

void ViewWidget::set_figure_type_to_bezierCurve(){
	if(draw_status == false)
        figure_type = kBezierCurve;
}

void ViewWidget::undo(){
	if( figureVec.size() > 0 ){
        delete figureVec.back();
        figureVec.pop_back();
    }
}

void ViewWidget::switch_drag(){
	canDrag = !canDrag;
	setMouseTracking(canDrag);
	for(int i=0; i<figureVec.size(); i++)
		figureVec[i]->setCanDrag(canDrag);
}

void ViewWidget::set_color(){
	QColor color = QColorDialog::getColor(Qt::white, this, tr("Set color of pen"));
	linePen.setColor(color);
}
void ViewWidget::set_width(){
	bool ok = false;
	int width = QInputDialog::getInt(this, tr("Set width of pen"), tr("width : 1 - 10"), 1, 1, 10, 1, &ok);
	if(ok)
		linePen.setWidth(width);
}
