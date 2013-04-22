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

int pointToPos(QGraphicsTextItem const *item, QPointF p);
/*:F pointToPos
 *:D Converts a point P in the textitem's coordinates to a cursor position
     (index into the text document).
 *:R Returns -1 if the point is not in the text block
*/
QPointF posToPoint(QGraphicsTextItem const *item, int i);
/*:F posToPoint
 *:D Converts a cursor position to a point in the textitem's coordinates.
     The point vertical position will be the midpoint of the line in which
     the cursor position resides.
 *:R Returns QPointF() if i is not a valid cursor position.
*/

#endif
