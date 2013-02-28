// PastePosition.C

#include "PastePosition.H"
#include "PageScene.H"
#include 
PastePosition::PastePosition(QGraphicsScene *scene) {
}

PastePosition::PastePosition(QGraphicsScene *scene, QPointF scenePos) {
}

bool PastePosition::inTextBlock() {
  return intextblock;
}

bool PastePosition::inText() {
  return textitem!=0;
}

class TextItem *PastePosition::textItem() {
  return textitem;
}

QPointF PastePosition::scenePos() {
  return scenepos;
}
