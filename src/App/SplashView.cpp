// SplashView.C

#include "SplashView.H"

#include <QGraphicsScene>
#include <QCloseEvent>
#include <QResizeEvent>

SplashView::SplashView() {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

SplashView::~SplashView() {
}

void SplashView::resizeEvent(QResizeEvent *e) {
  QGraphicsView::resizeEvent(e);
  if (!scene())
    return;
  QRectF sr = scene()->sceneRect();
  sr.adjust(2, 2, -2, -2); // make sure no borders show by default
  fitInView(sr, Qt::KeepAspectRatio);
}

void SplashView::closeEvent(QCloseEvent *e) {
  e->accept();
  emit closing();
}
