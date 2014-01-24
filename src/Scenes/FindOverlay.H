// FindOverlay.H

#ifndef FINDOVERLAY_H

#define FINDOVERLAY_H

#include "SceneOverlay.H"

class FindOverlay: public SceneOverlay {
  Q_OBJECT;
public:
  FindOverlay(class QGraphicsScene *scene, QString phrase);
  virtual ~FindOverlay();
protected:
  void timerEvent(QTimerEvent *); // self-destruct
};

#endif
