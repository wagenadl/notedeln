// ToolScene/ClockFace.h - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// ClockFace.h

#ifndef CLOCKFACE_H

#define CLOCKFACE_H

#include <QGraphicsObject>

class ClockFace: public QGraphicsObject {
  Q_OBJECT;
public:
  ClockFace(QGraphicsItem *parent=0);
  virtual ~ClockFace() {}
  void setScale(double r);
  void setForegroundColor(QColor fg);
  void setShadowColor(QColor sh);
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual void timerEvent(QTimerEvent *);
private:
  double scale;
  QColor foregroundColor;
  QColor shadowColor;
};

#endif
