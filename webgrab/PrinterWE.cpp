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
#include <QTemporaryFile>
#include <QProcess>
#include <QPrintDialog>
#include <QDebug>
#include <QWebEngineView>
#include <QApplication>
#include <stdio.h>
#include <QSvgGenerator>
#include <QFile>
#include <QPdfDocument>

PrinterWE::PrinterWE(QWebEngineView *src, Options const &opt):
  src(src), opt(opt) {
    havesize = false;
}

PrinterWE::~PrinterWE() {
}

void PrinterWE::display() {
  QSize si = src->page()->contentsSize().toSize();
  if (!si.isEmpty() && !havesize) {
      havesize = true;
    si += QSize(4, 4);
    if (si.width()>1100) {
      si.setWidth(1100);
      si.setHeight(si.height() + 30); // make space for scroll bar
    }
    if (si.height()>1100) {
      si.setHeight(1100);
      si.setWidth(si.width() + 30); // make space for scroll bar
    }
    src->resize(si); // adjust window size
  }
  QSize si2 = src->page()->contentsSize().toSize();
}


void PrinterWE::complete(bool ok) {
  qDebug() << "complete";
  if (!ok) {
    fprintf(stderr, "webgrab: Failed to load web page");
    QApplication::exit(2);
    return;
  }
  if (opt.out.isEmpty()) {
    src->show();
  } else {
    foreach (QString fn, opt.out) {
      if (fn.endsWith(".pdf")) {
        toPdf(fn);
        return;
      }
    }
    // no pdf is requested, we'll make a temporary pdf
    toPdf("");
  }
}

void PrinterWE::sizeChange(QSizeF const &) {
  display();
}

void PrinterWE::toPdf(QString fn) {
  if (opt.paginate)
    toMultiPagePdf(fn);
  else
    toSinglePagePdf(fn);
}


static bool allequalx(QImage const &img, int y, int x0, int x1, QRgb bg) {
  for (int x=x0; x<x1; x++)
    if (img.pixel(x,y)!=bg)
      return false;
  return true;
}

static bool allequaly(QImage const &img, int x, int y0, int y1, QRgb bg) {
  for (int y=y0; y<y1; y++)
    if (img.pixel(x,y)!=bg)
      return false;
  return true;
}


static QImage autotrim(QImage const &img) {
  int X = img.width();
  int Y = img.height();
  if (X<=1 || Y<=1)
    return img;
  QRgb bg(img.pixel(X-1, Y-1));
  int y1 = Y;
  while (y1>1) 
    if (allequalx(img, y1-1, 0, X, bg))
      --y1;
    else
      break;
  int x1 = X;
  while (x1>1)
    if (allequaly(img, x1-1, 0, y1, bg))
      --x1;
    else
      break;
  int x0 = 0;
  while (x0<x1-1)
    if (allequaly(img, x0, 0, y1, bg))
      x0++;
    else
      break;
  int y0 = 0;
  while (y0<y1-1)
    if (allequalx(img, y0, x0, x1, bg))
      y0++;
    else
      break;
  qDebug() << X << Y << x0 << y0 << x1 << y1;
  if (x0==0 && y0==0 && x1==X && y1==Y)
    return img;
  else
    return img.copy(x0, y0, x1-x0, y1-y0);
}



class Receiver {
public:
  Receiver(QString fn, Options const &opt):
    fn(fn), opt(opt) {
  }
  void operator()(QByteArray const &ar) {
    QFile *f = fn.isEmpty() ? new QTemporaryFile : new QFile(fn);
    if (f->open(QFile::WriteOnly)) {
      f->write(ar);
      f->close();
      convert(f->fileName());
      QApplication::exit(0);
    } else {
      qDebug() << "Cannot open" << fn;
      QApplication::exit(2);
    }
  }
  void convert(QString pdffn) {
    bool mustdo = false;
    foreach (QString ofn, opt.out) 
      if (ofn != pdffn)
        mustdo = true;
    if (!mustdo)
      return;
    
    QPdfDocument pdf;
    pdf.load(pdffn);
    if (pdf.pageCount()<1) {
      qDebug() << "No pages in pdf" << pdffn;
      QApplication::exit(2);
    }

    /* Ugly section follows to deal with unannounced API change in Qt PDF */
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    QSizeF ptsize = pdf.pagePointSize(0);
#else
    QSizeF ptsize = pdf.pageSize(0);
#endif
    /* End of ugly section */
    QSize outsize(opt.imSize,
                  int(opt.imSize*ptsize.height()/ptsize.width()));
    QImage img = autotrim(pdf.render(0, outsize));
    foreach (QString ofn, opt.out) 
      if (ofn != pdffn) 
        img.save(ofn);    
  }
private:
  QString fn;
  Options const &opt;
};

void PrinterWE::toMultiPagePdf(QString fn) {
  Receiver *recv = new Receiver(fn, opt);
  QPageLayout layout(QPageSize(QPageSize::Letter),
                     QPageLayout::Portrait,
                     QMarginsF());
  src->page()->printToPdf(*recv, layout);
}

void PrinterWE::toSinglePagePdf(QString fn) {
  toMultiPagePdf(fn);
  // single-page pdf not supported in WebEngine.
}


void PrinterWE::featureReq(QUrl const &url, QWebEnginePage::Feature f) {
    src->page()->setFeaturePermission(url, f,
                                      QWebEnginePage::PermissionDeniedByUser);
}

