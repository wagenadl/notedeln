// TextBlockItem.H

#ifndef TEXTBLOCKITEM_H

#define TEXTBLOCKITEM_H

#include "BlockItem.H"
#include "FutileMovementInfo.H"

#include <QTextCursor>

class TextBlockItem: public BlockItem {
  Q_OBJECT;
public:
public:
  TextBlockItem(class TextBlockData *data, Item *parent=0);
  virtual ~TextBlockItem();
  TextBlockData *data(); // benevolently hides inherited version
  QTextDocument *document() const;
  class TextItem *text();
  void setFocus();
  void clearFocus();
  void acceptFocus(QPointF);
  bool isEmpty() const;
  bool lastParIsEmpty() const;
  void dropEmptyLastPar();
  FutileMovementInfo const &lastFutileMovement() const;
  virtual QRectF boundingRect() const;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
  virtual void makeWritable();
signals:
  void futileMovement(); // up key on top line etc
private slots:
  void futileMovementKey(int, Qt::KeyboardModifiers);
private:
  void initializeFormat();
private:
  TextBlockData *data_; // we don't own
  class TextItem *item_; // we do own
  FutileMovementInfo fmi;
};

#endif
