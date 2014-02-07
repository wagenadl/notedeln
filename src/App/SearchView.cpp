// SearchView.cpp

#include "SearchView.H"
#include "SearchResultScene.H"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPrinter>
#include <QPrintDialog>

SearchView::SearchView(SearchResultScene *scene, QWidget *parent):
  QGraphicsView(parent), scene(scene) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  wheelDeltaAccum = 0;
  wheelDeltaStepSize = 120; // should get from notebook
  setScene(scene);
}

SearchView::~SearchView() {
  scene->deleteLater();
}

void SearchView::resizeEvent(QResizeEvent *e) {
  QGraphicsView::resizeEvent(e);
  QRectF sr = scene->sceneRect();
  sr.adjust(2, 2, -2, -2); // make sure no borders show by default
  fitInView(sr, Qt::KeepAspectRatio);
}

static void printme(BaseScene *scene) {
  QPrinter printer;
  QPrintDialog dialog(&printer, 0);
  dialog.setWindowTitle("Print scene");
  dialog.setOption(QAbstractPrintDialog::PrintToFile);
  if (dialog.exec() != QDialog::Accepted) 
    return;
  QPainter p;
  p.begin(&printer);
  scene->print(&printer, &p);
}


void SearchView::keyPressEvent(QKeyEvent *e) {
  bool take = true;
  switch (e->key()) {
  case Qt::Key_PageUp: case Qt::Key_Up: case Qt::Key_Backspace:
    scene->previousSheet();
    break;
  case Qt::Key_PageDown: case Qt::Key_Down: case Qt::Key_Space:
    scene->nextSheet();
    break;
  case Qt::Key_P:
    if (e->modifiers() & Qt::ControlModifier)
      printme(scene);
  default:
    take = false;
    break;
  }
  if (take)
    e->accept();
  else    
    QGraphicsView::keyPressEvent(e);
}

void SearchView::wheelEvent(QWheelEvent *e) {
  wheelDeltaAccum += e->delta();
  while (wheelDeltaAccum>=wheelDeltaStepSize) {
    wheelDeltaAccum -= wheelDeltaStepSize;
    scene->previousSheet();
  }
  while (wheelDeltaAccum<=-wheelDeltaStepSize) {
    wheelDeltaAccum += wheelDeltaStepSize;
    scene->nextSheet();
  }
}
