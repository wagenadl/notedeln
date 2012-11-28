// HoverRegion.H

#ifndef HOVERREGION_H

#define HOVERREGION_H

#include <QGraphicsObject>

class HoverRegion: public QGraphicsObject {
  Q_OBJECT;
public:
  HoverRegion(class MarkupData *md, class TextItem *item,
	      QGraphicsItem *parent);
  virtual ~HoverRegion();
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  virtual QPainterPath shape() const;
  void forgetBounds();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
private slots:
  void downloadFinished();
private:
  QString refText() const;
  void calcBounds() const;
  void openLink();
  void openArchive();
  void getArchiveAndPreview();
  bool hasArchive() const;
  bool hasPreview() const;
  class Resource *resource() const;
private:
  MarkupData *md;
  TextItem *ti;
  mutable QPainterPath bounds;
  class PreviewPopper *popper;
  mutable int start;
  mutable int end;
  QString lastRef;
  bool lastRefIsNew;
  bool busy;
};

#endif
