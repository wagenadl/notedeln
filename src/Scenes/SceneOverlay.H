// SceneOverlay.H

#ifndef SCENEOVERLAY_H

#define SCENEOVERLAY_H

#include <QGraphicsObject>

class SceneOverlay: public QGraphicsObject {
  Q_OBJECT;
public:
  SceneOverlay(QGraphicsItem *parent=0);
  virtual ~SceneOverlay() { }
  virtual QRectF boundingRect() const { return QRectF(); }
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget*) {}
public slots:
  void gotoSheet(int);
protected:
  void setSheetForChild(QGraphicsItem *c, int sh);
protected:
  QMap<QGraphicsItem *, int> sheetNos;
};

#endif
