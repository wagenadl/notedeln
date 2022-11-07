// App/SearchView.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    break;
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
  wheelDeltaAccum += e->angleDelta().y();
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
