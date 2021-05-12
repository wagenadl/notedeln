// Scenes/RoundedRect.H - This file is part of NotedELN

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

// RoundedRect.H

#ifndef ROUNDEDRECT_H

#define ROUNDEDRECT_H

#include <QGraphicsObject>

class RoundedRect: public QGraphicsObject {
  Q_OBJECT;
public:
  RoundedRect(QGraphicsItem *parent=0);
  virtual ~RoundedRect();
  void setBlackAlpha(double);
  void setWhiteAlpha(double);
  void resize(QSizeF);
  void resize(double, double);
  void setRadius(double);
  void setOffset(double);
  void setOffset(double, double);
protected:
  QRectF boundingRect() const;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *);
private:
  double width_;
  double height_;
  double radius_;
  double dx_;
  double dy_;
  double blackalpha_;
  double whitealpha_;
};

#endif
