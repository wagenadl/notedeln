// App/BookSplashItem.H - This file is part of eln

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

// BookSplashItem.H

#ifndef BOOKSPLASHITEM_H

#define BOOKSPLASHITEM_H

#include "RecentBooks.h"
#include <QGraphicsObject>

class BookSplashItem: public QGraphicsObject {
  Q_OBJECT;
public:
  static const int BOXWIDTH = 500;
  static const int BOXHEIGHT = 63;
  static const int SMALLBOXHEIGHT = 30;
  static const double BOXRAD = 3.0;
  static const double HOVERDX = 1.5;
  static const double HOVERDX1 = 0.5;
  static const double SHRINK = 1;
public:
  BookSplashItem(QString dirname, BookInfo const &info,
                 QGraphicsItem *parent=0);
  BookSplashItem(QString label,
                 QGraphicsItem *parent=0);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  QRectF boundingRect() const;
signals:
  void leftClick(QString); // dirname or label
  void rightClick(QString); // dirname or label
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  QString dirname;
  BookInfo info;
  bool hov;
};

#endif
