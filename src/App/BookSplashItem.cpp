// BookSplashItem.C

#include "BookSplashItem.H"
#include "Style.H"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#define BOXWIDTH 500
#define BOXHEIGHT 80
#define BOXRAD 3.0
#define HOVERDX 1.5
#define HOVERDX1 0.5
#define SHRINK 1

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
  /* Draw a box */
  QColor blk("black");
  blk.setAlphaF(0.25);
  QColor wht("white");
  wht.setAlphaF(hov ? 0.85 : 0.75);
  p->setPen(Qt::NoPen);
  p->setBrush(blk);
  p->drawRoundedRect(QRectF(SHRINK, SHRINK,
                            BOXWIDTH-2*SHRINK, BOXHEIGHT-2*SHRINK),
                     BOXRAD, BOXRAD);
    
  p->setBrush(wht);

  p->drawRoundedRect(QRectF(SHRINK+HOVERDX, SHRINK+HOVERDX,
                            BOXWIDTH-2*SHRINK-HOVERDX,
                            BOXHEIGHT-2*SHRINK-HOVERDX),
                     BOXRAD, BOXRAD);

  /* Draw contents */
  QFont f(Style::defaultStyle().font("splash-font"));

  // title
  double y = 4;
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
    p->setPen(QColor("#777777"));
  p->drawText(QRectF(8, 23, BOXWIDTH-16, 20),
              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
              dirname);

  // other items
  QString datefmt = Style::defaultStyle().string("splash-date-format");
  f.setPointSizeF(10);
  p->setFont(f);
  p->drawText(QRectF(8, 38, BOXWIDTH-16, 24),
              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
              "Author: " + info.author);
  p->drawText(QRectF(8, 55, BOXWIDTH-16, 24),
              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
              "Address: " + info.address);
  p->drawText(QRectF(8, 38, BOXWIDTH-16, 24),
              Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
              "Created: " + info.created.toString(datefmt));
  p->drawText(QRectF(8, 55, BOXWIDTH-16, 24),
              Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
              "Last modified: " + info.modified.toString(datefmt));
  if (info.accessed.isValid())
    p->drawText(QRectF(8, 21, BOXWIDTH-16, 24),
                Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
                "Last access: " + info.accessed.toString(datefmt));
}

QRectF BookSplashItem::boundingRect() const {
  return QRectF(0, 0, BOXWIDTH, BOXHEIGHT);
}
