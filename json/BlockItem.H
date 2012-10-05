// BlockItem.H

#ifndef BLOCKITEM_H

#define BLOCKITEM_H

#include <QGraphicsObject>

class BlockItem: public QGraphicsObject {
  Q_OBJECT;
public:
  BlockItem(class BlockData *data, class PageScene *parent);
  virtual ~BlockItem();
  BlockData *data();
  QRectF contentsRect() const; // in this item's coords
  QRectF contentsSceneRect() const; // in scene coords
  void moveContentsBy(double dx, double dy);
  virtual QRectF boundingRect() const;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
signals:
  void vboxChanged();
  void hboxChanged();
protected slots:
  void checkBbox();
  void checkVbox();
protected:
  void setMainChild(QGraphicsItem *);
private:
  BlockData *data_;
  QRectF oldBbox;
  QGraphicsItem *mainChild_;
  // QList<NoteItem *> notes_;
};

#endif
