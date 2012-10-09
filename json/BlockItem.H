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
  QRectF boundingRect() const;
  QRectF netBoundingRect() const; // w/o notes
  QRectF netSceneRect() const; // in scene coords
  QGraphicsItem *mainChild() const;
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
