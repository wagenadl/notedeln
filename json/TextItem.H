// TextItem.H

#ifndef TEXTITEM_H

#define TEXTITEM_H

#include <QGraphicsTextItem>
#include <QTextCursor>
#include "MarkupData.H"

class TextItem: public QGraphicsTextItem {
  Q_OBJECT;
public:
  TextItem(class TextData *data, QGraphicsItem *parent);
  ~TextItem();
  virtual bool makeWritable();
  void makeHardToWrite();
  bool allowParagraphs() const;
  void setAllowParagraphs(bool yes);
signals:
  void textChanged();
  void abandoned(); // escape pressed while empty
  void futileMovementKey(QTextCursor, int, Qt::KeyboardModifiers); 
  void enterPressed(); // emitted if creation of a new paragraph prevented
protected:
  void keyPressEvent(QKeyEvent *);
  void focusOutEvent(QFocusEvent *);
private slots:
  void docChange();
private:
  void initializeFormat();
  bool tryToPaste();
  bool trySimpleStyle(QString marker, MarkupData::Style type);
  bool tryScriptStyles();
  bool charBeforeIsLetter(int pos) const;
  bool charAfterIsLetter(int pos) const;
protected:
  TextData *data_;
  bool mayWrite;
  bool mayMark;
  bool allowParagraphs_;
  class TextMarkings *markings_;
};

#endif
