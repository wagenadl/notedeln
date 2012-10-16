// PageView.C

#include "PageView.H"

PageView::PageView(QWidget *parent): QGraphicsView(parent) {
  //  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  
}

PageView::~PageView() {
}

void PageView::resizeEvent(QResizeEvent *e) {
  QGraphicsView::resizeEvent(e);
  if (scene()) 
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}
