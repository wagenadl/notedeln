// ToolScene.cpp

#include "ToolScene.H"
#include "Toolbars.H"

ToolScene::ToolScene(Mode *mode, QObject *parent): QGraphicsScene(parent) {
  bars = new Toolbars(mode, 0);
  addItem(bars);
  bars->setPos(0, 0);
  setSceneRect(itemsBoundingRect().adjusted(-4, -39, 2, 2));
  setBackgroundBrush(QBrush(Qt::NoBrush));
}

ToolScene::~ToolScene() {
  // bars is deleted automatically because it has us as a parent
}
