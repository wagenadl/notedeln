// TextItem.H

#ifndef TEXTITEM_H

#define TEXTITEM_H

#include <QGraphicsTextItem>
#include <QTextCursor>
#include "MarkupData.H"
#include "Item.H"
#include "TextItemText.H"

class TextItem: public Item {
  Q_OBJECT;
public:
  TextItem(class TextData *data, Item *parent);
  ~TextItem();
  virtual void makeWritable();
  bool allowParagraphs() const;
  void setAllowParagraphs(bool yes);
  void setAllowMoves();
  MarkupData *addMarkup(MarkupData::Style t, int start, int end);
  void addMarkup(MarkupData *); // we appropriate the data!
  bool allowNotes() const;
  void setAllowNotes(bool y=true);
  TextData *data();
signals:
  void textChanged();
  void abandoned(); // escape pressed while empty
  void futileMovementKey(int, Qt::KeyboardModifiers);
  /* also emitted for enter press or for delete/backspace at end/start */
  void mousePress(QPointF, Qt::MouseButton);
  void refTextChange(QString oldText, QString newText);
private:
  friend class TextItemText;
  virtual bool mousePress(QGraphicsSceneMouseEvent *);
  virtual bool keyPress(QKeyEvent *);
  virtual bool focusIn(QFocusEvent *);
  virtual bool focusOut(QFocusEvent *);
protected slots:
  virtual void docChange();
  virtual void modifierChange(Qt::KeyboardModifiers);
  void linkFinished(QString);
public:
  void updateRefText(QString oldText, QString newText);
  // for use by TextMarkings to signal change of reference text
  QString markedText(MarkupData *);
  void hoverMove(QGraphicsSceneHoverEvent *);
  void setBoxVisible(bool);
private:
  void initializeFormat();
  MarkupData *markupAt(int pos, MarkupData::Style type);
  bool tryLink(MarkupData *md=0);
  bool tryFootnote();
  bool tryToPaste();
  bool trySimpleStyle(QString marker, MarkupData::Style type);
  bool tryScriptStyles();
  bool tryURL();
  bool tryCustomRef();
  bool charBeforeIsLetter(int pos) const;
  bool charAfterIsLetter(int pos) const;
  bool shouldResize(QPointF) const;
public: // pass-through
  void setFont(QFont f) { text->setFont(f); }
  QColor defaultTextColor() const { return text->defaultTextColor(); }
  void setDefaultTextColor(QColor c) { text->setDefaultTextColor(c); }
  QTextDocument *document() const { return text->document(); }
  void setTextWidth(double d);
  double textWidth() const { return text->textWidth(); }
  QTextCursor textCursor() const { return text->textCursor(); }
  void setTextCursor(QTextCursor c) { text->setTextCursor(c); }
public:
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
protected:
  TextData *data_;
  bool mayMark;
  bool mayNote;
  bool allowParagraphs_;
  class TextMarkings *markings_;
  class TextItemText *text;
  bool mayMove;
  QPointF cursorPos;
};

#endif
