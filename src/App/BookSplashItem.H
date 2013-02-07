// BookSplashItem.H

#ifndef BOOKSPLASHITEM_H

#define BOOKSPLASHITEM_H

#include "RecentBooks.H"
#include <QGraphicsObject>

class BookSplashItem: public QGraphicsObject {
  Q_OBJECT;
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
