// PageView.C

#include "PageView.H"
#include "App.H"
#include "ModSnooper.H"
#include <QKeyEvent>
#include <QDebug>

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
void PageView::keyPressEvent(QKeyEvent *e) {
  App::instance()->modSnooper()->keyPress(e->key());
  switch (e->key()) {
  case Qt::Key_PageUp:
    emit pgUp();
    e->accept();
    return;
  case Qt::Key_PageDown:
    emit pgDn();
    e->accept();
    return;
  case Qt::Key_Home:
    if (e->modifiers() & Qt::ControlModifier) {
      emit home();
      e->accept();
      return;
    }
  case Qt::Key_End:
    if (e->modifiers() & Qt::ControlModifier) {
      emit end();
      e->accept();
      return;
    }
  default:
    break;
  }
  QGraphicsView::keyPressEvent(e);
}

void PageView::keyReleaseEvent(QKeyEvent *e) {
  App::instance()->modSnooper()->keyRelease(e->key());
  QGraphicsView::keyReleaseEvent(e);
}
