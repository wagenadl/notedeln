// TextItemText.H

#ifndef TEXTITEMTEXT_H

#define TEXTITEMTEXT_H

#include <QGraphicsTextItem>

class TextItemText: public QGraphicsTextItem {
  Q_OBJECT;
public:
  TextItemText(class TextItem *parent);
  virtual ~TextItemText();
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
private:
  friend class TextItem;
  void internalKeyPressEvent(QKeyEvent *);
  TextItem *parent();
};

#endif
