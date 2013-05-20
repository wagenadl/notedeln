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
#include <stdio.h>
#include <QSvgGenerator>

Printer::Printer(QGraphicsWebView *src, Options const &opt):
  src(src), opt(opt) {
}

Printer::~Printer() {
}

void Printer::display() {
  QSize si = s->sceneRect().size().toSize() + QSize(2,2);
  if (si.width()>1100)
    si.setWidth(1100);
  if (si.height()>1100)
    si.setHeight(1100);
  v->resize(si);

  v->show();
}

void Printer::complete(bool ok) {
  if (!ok) {
    fprintf(stderr, "webgrab: Failed to load web page");
    QApplication::exit(2);
    return;
  }

  s = src->scene();
  Q_ASSERT(s);
  QList<QGraphicsView *>vv = s->views();
  Q_ASSERT(!vv.isEmpty());
  v = vv[0];
  Q_ASSERT(v);
  
  if (opt.out.isEmpty()) {
    display();
    return;
  } else {
    foreach (QString fn, opt.out) {
      if (fn.endsWith(".pdf"))
	toPdf(fn);
      else if (fn.endsWith(".svg"))
	toSvg(fn);
      else
	toImg(fn);
    }
    QApplication::exit(0);
  }
}

void Printer::toPdf(QString fn) {
  if (opt.paginate)
    toMultiPagePdf(fn);
  else
    toSinglePagePdf(fn);
}

void Printer::toMultiPagePdf(QString fn) {
  qDebug() << "Rendering to multipage pdf" << fn;
  QPrinter printer;
  printer.setOutputFileName(fn);
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
    s->setSceneRect(clip);
    s->render(&p);
  }
  p.end();
}

void Printer::toSinglePagePdf(QString fn) {
  qDebug() << "Rendering to single page pdf" << fn;
  QPrinter printer;
  printer.setOutputFileName(fn);
  printer.setFullPage(true);
  printer.setPaperSize(s->sceneRect().size(), QPrinter::Point);
  QPainter p;
  p.begin(&printer);
  s->render(&p);
  p.end();
}

void Printer::toSvg(QString fn) {
  qDebug() << "Rendering to svg" << fn;
  QSvgGenerator printer;
  printer.setFileName(fn);
  printer.setResolution(90);
  QSizeF si = s->sceneRect().size()*90./72.;
  printer.setSize(si.toSize());
  
  QPainter p;
  p.begin(&printer);
  s->render(&p);
  p.end();
}

void Printer::toImg(QString fn) {
  qDebug() << "Rendering to image file" << fn;
  double maxDim = opt.imSize;
  QSizeF si = s->sceneRect().size();
  double w = si.width();
  double h = si.height();
  double scl = w>h ? maxDim/w : maxDim/h;
  
  QPixmap printer(scl*w, scl*h);
  printer.fill();
  QPainter p;
  p.begin(&printer);
  s->render(&p);
  p.end();

  QPixmap copy = printer.copy(0, 0,
			      printer.width()-1, printer.height()-1);
  // trim off one pixel
  
  if (!copy.save(fn)) {
    fprintf(stderr, "Failed to save image");
    QApplication::exit(2);
  }
}


  
