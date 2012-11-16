// Printer.C

#include "Printer.H"
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWebView>
#include <QApplication>
#include <QRectF>
#include <QSizeF>
#include <QPointF>

Printer::Printer(QGraphicsWebView *src, QString ofn): src(src), ofn(ofn) {
 paginate_ = false;
}

Printer::~Printer() {
}

void Printer::paginate(bool b) {
  paginate_ = b;
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
    QPrinter printer;
    printer.setOutputFileName(ofn);
    if (paginate_) {
      QSizeF pp = printer.paperSize(QPrinter::Point);
      qreal l, t, r, b;
      printer.getPageMargins(&l, &t, &r, &b, QPrinter::Point);
      pp -= QSizeF(l+r, t+b);
      QRectF sr = s->sceneRect();
      double pw = pp.width();
      double sw = sr.width();
      double dy = pp.height();
      if (sw>pw)
	dy *= sw/pw;

      QPainter p;
      p.begin(&printer);
      for (double y0 = sr.top(); y0<sr.bottom(); y0 += dy) {
	if (y0!=sr.top())
	  printer.newPage();
	QRectF clip(QPointF(sr.left(), y0), QSizeF(sr.width(), dy));
	// to properly clip, I would need a clipper item
	//QPainterPath pp; pp.addRect(clip);
	//src->setClipPath(pp);
	//src->setFlags(QGraphicsItem::ItemClipsToShape|QGraphicsItem::ItemClipsChildrenToShape);
	s->setSceneRect(clip);
	s->render(&p);
      }
      p.end();
    } else {
      printer.setFullPage(true);
      printer.setPaperSize(s->sceneRect().size(), QPrinter::Point);
      QPainter p;
      p.begin(&printer);
      s->render(&p);
      p.end();
    }
    QApplication::quit();
  }
}

    
