// webgrab/Printer.cpp - This file is part of eln

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

// Printer.C

#include "Printer.h"
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
  s = 0;
  v = 0;
}

Printer::~Printer() {
}

void Printer::display() {
  Q_ASSERT(v);
  
  QSize si = s->sceneRect().size().toSize() + QSize(2,2);
  if (si.width()>1100)
    si.setWidth(1100);
  if (si.height()>1100)
    si.setHeight(1100);
  v->resize(si); // adjust window size

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

  QPixmap foo(100, 100);
  QPainter ptr(&foo);
  s->render(&ptr); // this forces Qt to actually calculate bboxes
  
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
    s->render(&p, QRectF(), clip);
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
  double maxDim = opt.imSize;
  QSizeF si = s->sceneRect().size();
  double w = si.width();
  double h = si.height();
  if (w*h>0) {
    double rat = w/h;
    double wo, ho;
    if (rat>2.) {
      // crop somehow
      ho = maxDim/2;
      wo = rat*ho;
    } else if (rat<2/3.) {
      // crop somehow
      wo = maxDim*2/3;
      ho = wo/rat;
    } else {
      double scl = w>h ? maxDim/w : maxDim/h;
      wo = scl*w;
      ho = scl*h;
    }
    QPixmap printer(wo, ho);
    printer.fill();
    QPainter p;
    p.begin(&printer);
    s->render(&p);
    p.end();

    QRect crop(0, 0, wo, ho);
    if (rat>2.) 
      crop = QRect((wo-2*ho)/2, 0, 2*ho, ho);
    else if (rat<2/3.)
      crop = QRect(0, 0, wo, 1.5*wo);
    QPixmap copy = printer.copy(crop.adjusted(0, 0, -1, -1));
    // trim off one pixel [why? 1/29/16]
    if (!copy.save(fn)) {
      fprintf(stderr, "Failed to save image");
      QApplication::exit(2);
    }
  } else {
    fprintf(stderr, "Saving empty image");
    QPixmap nul(0,0);
    nul.save(fn);
  }
}


  
