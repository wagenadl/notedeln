// PreviewPopper.C

#include "PreviewPopper.H"
#include "Resource.H"
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>

PreviewPopper::PreviewPopper(Resource *res,
			     QPoint center, QObject *parent):
  QObject(parent), res(res), center(center) {
  widget = 0;
  timerID = startTimer(100);
}

PreviewPopper::~PreviewPopper() {
  killTimer(timerID);
  if (widget)
    delete widget;
}

void PreviewPopper::timerEvent(QTimerEvent *) {
  killTimer(timerID);
  popup();
}

QWidget *PreviewPopper::popup() {
  if (!res->hasPreview())
    return 0;
  
  if (widget) {
    smartPosition();
    widget->show();
    return widget;
  }

  QPixmap p;
  if (!p.load(res->previewPath()))
    return 0;
  
  QLabel *label = new QLabel(0, Qt::FramelessWindowHint);
  label->setPixmap(p);
  label->resize(label->sizeHint());
  widget = label;
  smartPosition();
  widget->show();
  return widget;
}

void PreviewPopper::smartPosition() {
  Q_ASSERT(widget);
  
  QRect desktop = QApplication::desktop()->screenGeometry();

  QSize s = widget->frameSize();

  /* We will attempt to position the popup so that it is away from the
     mouse pointer. There are several options:
     (1) below the mouse pointer and sticking out to the left and right
     (2) above the mouse pointer and sticking out to the left and right
     (3) to the right of the mouse pointer and stickout out up and down
     (4) to the left of  the mouse pointer and stickout out up and down
     We try those in order and maximize how much of the popup fits on the
     screen.
  */
  QPoint dest;
  QPoint bestDest;
  QRectF ir;
  double area;
  double bestArea = 0;
  int dy = 25;
  int dx = 50;

  // below
  dest = center + QPoint(-s.width()/3, dy);
  if (dest.x()+s.width()>desktop.right())
    dest.setX(desktop.right()-s.width());
  if (dest.x()<desktop.left())
    dest.setX(desktop.left());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }

  // above
  dest = center + QPoint(-s.width()/3, -dy-s.height());
  if (dest.x()+s.width()>desktop.right())
    dest.setX(desktop.right()-s.width());
  if (dest.x()<desktop.left())
    dest.setX(desktop.left());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }

  // to the right
  dest = center + QPoint(dx, -s.height()/3);
  if (dest.y()+s.height()>desktop.bottom())
    dest.setY(desktop.bottom()-s.height());
  if (dest.y()<desktop.top())
    dest.setY(desktop.top());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }

  // to the left
  dest = center + QPoint(-dx-s.width(), -s.height()/3);
  if (dest.y()+s.height()>desktop.bottom())
    dest.setY(desktop.bottom()-s.height());
  if (dest.y()<desktop.top())
    dest.setY(desktop.top());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }

  widget->move(bestDest);
}

