// TextItemText.H

#ifndef TEXTITEMTEXT_H

#define TEXTITEMTEXT_H

#include <QGraphicsTextItem>

class TextItemText: public QGraphicsTextItem {
  Q_OBJECT;
public:
  TextItemText(class TextItem *parent);
  virtual ~TextItemText();
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  void setBoxVisible(bool);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
  void hoverMoveEvent(QGraphicsSceneHoverEvent *);
private:
  friend class TextItem;
  void internalKeyPressEvent(QKeyEvent *);
  TextItem *parent();
private:
  bool forcebox;
};

#endif
