// App/ToolItem.H - This file is part of eln

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

// ToolItem.H

#ifndef TOOLITEM_H

#define TOOLITEM_H

#include <QGraphicsObject>
#include <QPointer>

class ToolItem: public QGraphicsObject {
  Q_OBJECT;
public:
  ToolItem();
  void setBalloonHelpText(QString);
  virtual ~ToolItem();
  bool isSelected() const;
  void setSelected(bool);
  void setEnabled(bool=true);
  void setDisabled(bool=true);
  bool isEnabled() const;
  void setSvg(QString filename);
  virtual void paintContents(QPainter *);
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
signals:
  void leftClick(Qt::KeyboardModifiers);
  void rightClick(Qt::KeyboardModifiers);
  void release();
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  void hoverMoveEvent(QGraphicsSceneHoverEvent *);
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private slots:
  void popup();
private:
  bool sel;
  bool hov;
  bool ena;
  class QSvgRenderer *svg;
  class QTimer *popupDelay;
  QString helpText;
  QPointF popupPos;
  QPointer<QGraphicsObject> balloon;
};

#endif
