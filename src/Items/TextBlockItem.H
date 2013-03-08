// TextBlockItem.H

#ifndef TEXTBLOCKITEM_H

#define TEXTBLOCKITEM_H

#include "BlockItem.H"
#include "FutileMovementInfo.H"
#include "TextBlockData.H"
#include <QTextCursor>

class TextBlockItem: public BlockItem {
  Q_OBJECT;
public:
  TextBlockItem(TextBlockData *data, Item *parent=0);
  virtual ~TextBlockItem();
  DATAACCESS(TextBlockData);
  QTextDocument *document() const;
  class TextItem *text();
  void setFocus();
  void clearFocus();
  void acceptFocus(QPointF);
  bool isEmpty() const;
  bool lastParIsEmpty() const;
  void dropEmptyLastPar();
  FutileMovementInfo const &lastFutileMovement() const;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
  virtual void makeWritable();
signals:
  void futileMovement(); // up key on top line etc
private slots:
  void futileMovementKey(int, Qt::KeyboardModifiers);
private:
  void initializeFormat();
private:
  class TextItem *item_; // we do own
  FutileMovementInfo fmi;
};

#endif
