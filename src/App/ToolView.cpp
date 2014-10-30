// ToolView.cpp

#include "ToolView.H"
#include "ToolScene.H"
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsItem>
#include <QDragEnterEvent>

ToolView::ToolView(Mode *mode, QWidget *parent): QGraphicsView(parent) {
  tools = new ToolScene(mode, this);
  setStyleSheet("background: transparent");
  setFrameShape(NoFrame);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setDragMode(NoDrag);
  setFocusPolicy(Qt::NoFocus);
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  
  setScene(tools);
  connect(tools, SIGNAL(changed(const QList<QRectF> &)),
	  SLOT(autoMask()));

  setAcceptDrops(true);
}

ToolView::~ToolView() {
  // scene is deleted automatically because it has us as a QObject parent
}

Toolbars *ToolView::toolbars() {
  return tools->toolbars();
}
		   
void ToolView::setScale(double x) {
  resetTransform();
  scale(x, x);
  autoMask();
}

void ToolView::autoMask() {
  QRegion reg;
  foreach (QGraphicsItem *i, tools->items()) {
    reg |= mapFromScene(i->sceneBoundingRect().adjusted(-2,-2,4,4));
  }
  setMask(reg);
}

void ToolView::mousePressEvent(QMouseEvent *e) {
  QPointF x = mapToScene(e->pos());
  if (!tools->itemAt(x, QTransform()))
    e->ignore();
  QGraphicsView::mousePressEvent(e);
}

void ToolView::dragEnterEvent(QDragEnterEvent *e) {
  e->acceptProposedAction();
}  

void ToolView::dragMoveEvent(QDragMoveEvent *e) {
  e->acceptProposedAction();
}  

void ToolView::dropEvent(QDropEvent *e) {
  emit drop(*e);
  e->acceptProposedAction();
}  
