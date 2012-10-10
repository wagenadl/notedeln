// TextBlockItem.H

#ifndef TEXTBLOCKITEM_H

#define TEXTBLOCKITEM_H

#include "BlockItem.H"

#include <QTextCursor>

class TextBlockItem: public BlockItem {
  Q_OBJECT;
public:
  class FutileMovementInfo {
  public:
    QPointF pos() const;
    QPointF scenePos() const;
    int key() const;
    Qt::KeyboardModifiers modifiers() const;
  private:
    QPointF pos_;
    QPointF scenePos_;
    int key_;
    Qt::KeyboardModifiers modifiers_;
    friend class TextBlockItem;
  };
public:
  TextBlockItem(class TextBlockData *data, class PageScene *parent);
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
signals:
  void abandoned(); // escape pressed while empty
  void futileMovement(); // up key on top line etc
  void newParagraph(); // emitted after the creation of a new paragraph
private slots:
  void futileMovementKey(QTextCursor, int, Qt::KeyboardModifiers);
private:
  TextBlockData *data_; // we don't own
  class TextItem *item_; // we do own
  FutileMovementInfo fmi;
};

#endif
