// webgrab/PrinterWE.cpp - This file is part of eln

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

// PrinterWE.C

#include "PrinterWE.h"
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QDebug>
#include <QWebEngineView>
#include <QApplication>
#include <stdio.h>
#include <QSvgGenerator>
#include <QFile>

PrinterWE::PrinterWE(QWebEngineView *src, Options const &opt):
  src(src), opt(opt) {
    havesize = false;
}

PrinterWE::~PrinterWE() {
}

void PrinterWE::display() {
  QSize si = src->page()->contentsSize().toSize();
  qDebug() << "display" << si;
  if (!si.isEmpty() && !havesize) {
      havesize = true;
    si += QSize(2,2);
    if (si.width()>1100)
      si.setWidth(1100);
    if (si.height()>1100)
      si.setHeight(1100);
    qDebug() << "display ->" << si;
    src->resize(si); // adjust window size
  }
  QSize si2 = src->page()->contentsSize().toSize();
  qDebug() << "display" << si2;

}


void PrinterWE::complete(bool ok) {
  if (!ok) {
    fprintf(stderr, "webgrab: Failed to load web page");
    QApplication::exit(2);
    return;
  }
  qDebug() << "complete";
  src->show();

}

void PrinterWE::sizeChange(QSizeF const &s) {
    qDebug() << "size change " << s;
    display();
    if (opt.out.isEmpty()) {
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
  //    QApplication::exit(0);
    }
}

void PrinterWE::toPdf(QString fn) {
  if (opt.paginate)
    toMultiPagePdf(fn);
  else
    toSinglePagePdf(fn);
}

class Receiver {
public:
  Receiver(QString fn): fn(fn) {
  }
  void operator()(QByteArray const &ar) {
    QFile f(fn);
    if (!f.open(QFile::WriteOnly)) {
        qDebug() << "Cannot open" << fn;
    }
    f.write(ar);
    f.close();
    qDebug() << "Hello world" << ar.size();
    QApplication::exit(0);
  }
private:
  QString fn;
};

void PrinterWE::toMultiPagePdf(QString fn) {
  qDebug() << "tomultipagepdf" << fn;
  Receiver *recv = new Receiver(fn);
  src->page()->printToPdf(*recv, QPageLayout(QPageSize(QPageSize::Letter), QPageLayout::Portrait, QMarginsF()));
}

void PrinterWE::toSinglePagePdf(QString fn) {
  qDebug() << "Rendering to single page pdf" << fn << src->page()->contentsSize() << src->size();
  QPrinter printer;
  printer.setOutputFileName(fn);
  printer.setFullPage(true);
  printer.setPaperSize(src->page()->contentsSize(), QPrinter::Point);
  QPainter p;
  p.begin(&printer);
  src->render(&p);
  p.end();
}

void PrinterWE::toSvg(QString fn) {
  QSvgGenerator printer;
  printer.setFileName(fn);
  printer.setResolution(90);
  QSizeF si = src->page()->contentsSize()*90./72.;
  printer.setSize(si.toSize());
  
  QPainter p;
  p.begin(&printer);
  src->render(&p);
  p.end();
}

void PrinterWE::toImg(QString fn) {
  double maxDim = opt.imSize;
  QSizeF si = src->page()->contentsSize();
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
    src->render(&p);
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

void PrinterWE::featureReq(QUrl const &url, QWebEnginePage::Feature f) {
    src->page()->setFeaturePermission(url, f, QWebEnginePage::PermissionDeniedByUser);
}

