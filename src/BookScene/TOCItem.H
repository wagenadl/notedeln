// TOCItem.H

#ifndef TOCITEM_H

#define TOCITEM_H

#include "Item.H"
#include <QGraphicsObject>

class TOCItem: public QGraphicsObject, public Item {
  Q_OBJECT;
public:
  TOCItem(class TOCEntry *data, class TOCScene *parent);
  virtual ~TOCItem();
  TOCEntry *data();
  QRectF boundingRect() const;
signals:
  void vboxChanged();
public slots:
  void entryChanged();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  bool resetVBox(); // return true iff actual change
private:
  void makeItems();
  void fillItems();
  void positionItems();
private:
  QGraphicsTextItem *dateItem;
  QGraphicsTextItem *titleItem;
  QGraphicsTextItem *pgnoItem;
  TOCEntry *data_;
  TOCScene *scene_;
};

#endif
