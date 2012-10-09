// TextBlockTextItem.H

#ifndef TEXTBLOCKTEXTITEM_H

#define TEXTBLOCKTEXTITEM_H

#include <QGraphicsTextItem>
#include <QTextCursor>

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
private:
  TextBlockData *data_;
  class TextMarkings *markings_;
};

#endif
