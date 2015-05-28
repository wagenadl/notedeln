// App/BookSplashItem.cpp - This file is part of eln

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

// BookSplashItem.C

#include "BookSplashItem.h"
#include "Style.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

BookSplashItem::BookSplashItem(QString dirname, BookInfo const &info,
                               QGraphicsItem *parent):
  QGraphicsObject(parent), dirname(dirname), info(info) {
  hov = false;
  setAcceptHoverEvents(true);
}

BookSplashItem::BookSplashItem(QString label,
                               QGraphicsItem *parent):
  QGraphicsObject(parent) {
  info.title = label;
  hov = false;
  setAcceptHoverEvents(true);
}


void BookSplashItem::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  hov = true;
  update();
}

void BookSplashItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  hov = false;
  update();
}

void BookSplashItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  switch (e->button()) {
  case Qt::LeftButton:
    emit leftClick(dirname.isEmpty() ? info.title : dirname);
    e->accept();
    break;
  case Qt::RightButton:
    emit rightClick(dirname.isEmpty() ? info.title : dirname);
    e->accept();
    break;
  default:
    e->ignore();
    break;
  }
}
    
void BookSplashItem::paint(QPainter *p,
                           const QStyleOptionGraphicsItem *, QWidget *) {
  double height = boundingRect().height();

  /* Draw a box */
  QColor blk("black");
  blk.setAlphaF(0.25);
  QColor wht("white");
  wht.setAlphaF(hov ? 0.85 : 0.75);
  p->setPen(Qt::NoPen);
  p->setBrush(blk);
  p->drawRoundedRect(QRectF(SHRINK, SHRINK,
                            BOXWIDTH-2*SHRINK, height-2*SHRINK),
                     BOXRAD, BOXRAD);
    
  p->setBrush(wht);

  p->drawRoundedRect(QRectF(SHRINK+HOVERDX, SHRINK+HOVERDX,
                            BOXWIDTH-2*SHRINK-HOVERDX,
                            height-2*SHRINK-HOVERDX),
                     BOXRAD, BOXRAD);

  /* Draw contents */
  QFont f(Style::defaultStyle().font("splash-font"));

  // title
  double y = dirname.isEmpty() ? 4 /*8*/ : 4;
  f.setPointSizeF(14);
  p->setFont(f);
  p->setBrush(QBrush(Qt::NoBrush));
  p->setPen(QColor("black"));
  p->drawText(QRectF(8, y, BOXWIDTH-16, 20),
              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
              info.title);

  if (dirname.isEmpty())
    return;
  
  // filename
  f.setPointSizeF(8);
  p->setFont(f);
  if (!hov)
    p->setPen(QColor("#444444"));
  p->drawText(QRectF(8, 24, BOXWIDTH-16, 20),
              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
              dirname);

  // other items
  QString datefmt = Style::defaultStyle().string("splash-date-format");
  f.setPointSizeF(10);
  p->setFont(f);
  p->drawText(QRectF(8, 38, BOXWIDTH-16, 24),
              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
              "Author: " + info.author);
  //p->drawText(QRectF(8, 55, BOXWIDTH-16, 24),
  //            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
  //            "Address: " + info.address);
  p->drawText(QRectF(8, 38-17, BOXWIDTH-16, 24),
              Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
              "Created: " + info.created.toString(datefmt));
  p->drawText(QRectF(8, 38, BOXWIDTH-16, 24),
              Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
              "Last modified: " + info.modified.toString(datefmt));
  if (info.accessed.isValid())
    p->drawText(QRectF(8, 38-2*17, BOXWIDTH-16, 24),
                Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
                "Last access: " + info.accessed.toString(datefmt));
}

QRectF BookSplashItem::boundingRect() const {
  double height = dirname.isEmpty() ? SMALLBOXHEIGHT : BOXHEIGHT;
  return QRectF(0, 0, BOXWIDTH, height);
}
