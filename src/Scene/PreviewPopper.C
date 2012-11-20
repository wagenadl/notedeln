// PreviewPopper.C

#include "PreviewPopper.H"
#include "ResourceManager.H"
#include <QApplication>
#include <QDesktopWidget>

PreviewPopper::PreviewPopper(ResourceManager *resmgr, QString resname,
	      QPoint center, QObject *parent):
  resmgr(resmgr), resname(resname), center(center) {
  widget = 0;
  startTimer(100);
}

PreviewPopper::~PreviewPopper() {
  stopTimer();
  if (widget)
    delete widget;
}

void PreviewPopper::timerEvent(QTimerEvent *) {
  stopTimer();
  popup();
}

QWidget *PreviewPopper::popup() {
  if (widget) {
    positionAndShow();
    return widget;
  }

  QPixmap p;
  if (!p.load(resmgr->thumbPath(resname)))
    return 0;
  
  QLabel *label = new QLabel();
  label->setPixmap(p);
  widget = label;
  positionAndShow();
  return widget;
}

void PreviewPopper::positionAndShow() {
  Q_ASSERT(widget);
  
  QWidget *p = dynamic_cast<QWidget*>(parent());
  if (!p)
    return; // forget it if our parent is no longer around
  
  QRect desktop = QApplication::desktop()->screenGeometry(p);

  QSize size = widget->frameSize();

  /* We will attempt to position the popup so that it is away from the
     mouse pointer. There are several options:
     (1) below the mouse pointer and sticking out to the left and right
     (2) above the mouse pointer and sticking out to the left and right
     (3) to the right of the mouse pointer and stickout out up and down
     (4) to the left of  the mouse pointer and stickout out up and down
     We try those in order and maximize how much of the popup fits on the
     screen.
  */
  QPoint bestPos;
  double bestArea = 0;
  
  QPoint attempt = center + QPoint(-size.width()/3, 64);
  QRect onScreen 
