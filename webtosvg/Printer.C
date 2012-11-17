// Printer.C

#include "Printer.H"
#include <QSvgGenerator>
#include <QPainter>
#include <QPrintDialog>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWebView>
#include <QApplication>

Printer::Printer(QGraphicsWebView *src, QString ofn): src(src), ofn(ofn) {
}

Printer::~Printer() {
}

void Printer::complete(bool ok) {
  if (!ok) {
    qDebug() << "Failed to load web page";
    QApplication::exit(2);
    return;
  }

  QGraphicsScene *s = src->scene();

  if (ofn.isEmpty()) {
    QGraphicsView *v = s->views()[0];
    QSize si = s->sceneRect().size().toSize() + QSize(2,2);
    
    if (si.width()>1100)
      si.setWidth(1100);
    if (si.height()>1100)
      si.setHeight(1100);
    v->resize(si);
    v->show();
  } else {
    QSvgGenerator printer;
    printer.setFileName(ofn);
    printer.setResolution(90);
    printer.setViewBox(QRectF(s->sceneRect().topLeft()*90./72.,
			      s->sceneRect().bottomRight()*90./72.));
    QPainter p;
    p.begin(&printer);
    p.scale(90./72., 90./72.);
    s->render(&p);
    p.end();
    QApplication::quit();
  }
}

    
