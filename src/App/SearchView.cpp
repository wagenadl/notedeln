// SearchView.cpp

#include "SearchView.h"
#include "SearchResultScene.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>
#include "SheetScene.h"

SearchView::SearchView(SearchResultScene *scene, QWidget *parent):
  QGraphicsView(parent), srs(scene) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameStyle(Raised | StyledPanel);
  setDragMode(NoDrag);
  wheelDeltaAccum = 0;
  wheelDeltaStepSize = 120; // should get from notebook
  gotoSheet(0);
}

SearchView::~SearchView() {
  srs->deleteLater();
}

void SearchView::resizeEvent(QResizeEvent *e) {
  QGraphicsView::resizeEvent(e);
  QRectF sr = scene()->sceneRect();
  sr.adjust(1, 1, -2, -2); // make sure no borders show by default
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
    gotoSheet(currentSheet-1);
    break;
  case Qt::Key_PageDown: case Qt::Key_Down: case Qt::Key_Space:
    gotoSheet(currentSheet+1);
    break;
  case Qt::Key_P:
    if (e->modifiers() & Qt::ControlModifier)
      printme(srs);
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
    gotoSheet(currentSheet-1);
  }
  while (wheelDeltaAccum<=-wheelDeltaStepSize) {
    wheelDeltaAccum += wheelDeltaStepSize;
    gotoSheet(currentSheet+1);
  }
}

void SearchView::gotoSheet(int n) {
  if (n<0)
    return;
  if (n>=srs->sheetCount())
    return;
  setScene(srs->sheet(n));
  currentSheet = n;
  QRectF sr = scene()->sceneRect();
  sr.adjust(1, 1, -2, -2); // make sure no borders show by default
  fitInView(sr, Qt::KeepAspectRatio);
}
