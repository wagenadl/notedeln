// TextItem.H

#ifndef TEXTITEM_H

#define TEXTITEM_H

#include <QGraphicsTextItem>
#include <QTextCursor>
#include "MarkupData.H"
#include "Item.H"

class TextItem: public QGraphicsTextItem, public Item {
  Q_OBJECT;
public:
  TextItem(class TextData *data, Item *parent);
  ~TextItem();
  virtual void makeWritable();
  bool allowParagraphs() const;
  void setAllowParagraphs(bool yes);
signals:
  void textChanged();
  void abandoned(); // escape pressed while empty
  void futileMovementKey(QTextCursor, int, Qt::KeyboardModifiers); 
  void enterPressed(); // emitted if creation of a new paragraph prevented
  void mousePress(QPointF, Qt::MouseButton);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void focusOutEvent(QFocusEvent *);
protected slots:
  virtual void docChange();
  void modifierChange(Qt::KeyboardModifiers);
private:
  void initializeFormat();
  bool tryToPaste();
  bool trySimpleStyle(QString marker, MarkupData::Style type);
  bool tryScriptStyles();
  bool tryURL();
  bool tryCustomRef();
  bool charBeforeIsLetter(int pos) const;
  bool charAfterIsLetter(int pos) const;
protected:
  TextData *data_;
  bool mayMark;
  bool allowParagraphs_;
  class TextMarkings *markings_;
};

#endif
