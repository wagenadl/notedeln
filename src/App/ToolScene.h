// ToolScene.H

#ifndef TOOLSCENE_H

#define TOOLSCENE_H

#include <QGraphicsScene>

class ToolScene: public QGraphicsScene {
  Q_OBJECT;
public:
  ToolScene(class Mode *mode, QObject *parent=0);
  virtual ~ToolScene();
public:
  class Toolbars *toolbars() { return bars; }
private:
  class Toolbars *bars;
};

#endif
