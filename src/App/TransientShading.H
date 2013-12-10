// TransientShading.H - This file is part of eln

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

#ifndef TRANSIENTSHADING_H

#define TRANSIENTSHADING_H

#include <QGraphicsObject>

class TransientShading: public QGraphicsObject {
  Q_OBJECT;
public:
  TransientShading(class TextItem *item, int start, int end,
		   QGraphicsItem *parent);
  virtual ~TransientShading();
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  virtual QPainterPath shape() const;
private:
  void calcBounds();
private:
  TextItem *ti;
  int start;
  int end;
  QPainterPath bounds;
};

#endif
