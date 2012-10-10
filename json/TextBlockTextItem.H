// TextBlockTextItem.H

#ifndef TEXTBLOCKTEXTITEM_H

#define TEXTBLOCKTEXTITEM_H

#include <QGraphicsTextItem>
#include <QTextCursor>
#include "MarkupData.H"

class TextBlockTextItem: public QGraphicsTextItem {
  Q_OBJECT;
public:
  TextBlockTextItem(class TextBlockData *data, class TextBlockItem *parent);
  ~TextBlockTextItem();
signals:
  void textChanged();
  void abandoned(); // escape pressed while empty
  void futileMovementKey(QTextCursor, int, Qt::KeyboardModifiers); 
  void newParagraph(); // emitted after the creation of a new paragraph
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
private:
  TextBlockData *data_;
  class TextMarkings *markings_;
};

#endif
