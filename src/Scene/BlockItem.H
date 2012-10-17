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
  virtual QRectF netBoundingRect() const; // w/o notes
  QRectF netSceneRect() const; // in scene coords
signals:
  void vboxChanged();
public slots:
  void checkVbox();
protected:
  void resetBbox(); // size change w/o emitting signals
private:
  BlockData *data_;
  QRectF oldBbox;
  // QList<NoteItem *> notes_;
};

#endif
