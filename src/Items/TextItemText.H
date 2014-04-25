// Items/TextItemText.H - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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
  virtual QPainterPath shape() const;
  void setClip(QRectF);
  void unclip();
signals:
  void invisibleFocus(QPointF);
public: // for EntryScene. Ugly.
  void keyPressEvent(QKeyEvent *);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
  void hoverMoveEvent(QGraphicsSceneHoverEvent *);
  void dragEnterEvent(class QGraphicsSceneDragDropEvent *);
  void dropEvent(class QGraphicsSceneDragDropEvent *);
private:
  void ensureFocusVisible();
  friend class TextItem;
  void internalKeyPressEvent(QKeyEvent *);
  TextItem *parent();
private:
  bool forcebox;
  QPainterPath clipp;
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
