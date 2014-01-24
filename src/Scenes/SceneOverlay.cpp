// SceneOverlay.cpp

#include "SceneOverlay.H"

SceneOverlay::SceneOverlay(QGraphicsItem *parent): QGraphicsObject(parent) {
}

void SceneOverlay::gotoSheet(int s) {
  foreach (QGraphicsItem *i, sheetNos.keys())
    if (sheetNos[i] == s)
      i->show();
    else
      i->hide();
}

void SceneOverlay::setSheetForChild(QGraphicsItem *c, int s) {
  sheetNos[c] = s;
}

