// ToolItem.H

#ifndef TOOLITEM_H

#define TOOLITEM_H

#include <QGraphicsObject>

class ToolItem: public QGraphicsObject {
  Q_OBJECT;
public:
  ToolItem();
  virtual ~ToolItem();
  bool isSelected() const;
  void setSelected(bool);
  void setSvg(QString filename);
  virtual void paintContents(QPainter *);
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
signals:
  void leftClick();
  void rightClick();
  void release();
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private:
  bool sel;
  bool hov;
  class QSvgRenderer *svg;
};

#endif
