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
    bool stayalive = false;
    foreach (QString fn, opt.out) {
      if (fn.endsWith(".pdf")) {
        stayalive = true;
        toPdf(fn);
      } else if (fn.endsWith(".svg")) {
        toSvg(fn);
      } else {
        toImg(fn);
      }
    }
    if (!stayalive)
      QApplication::exit(0);
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
    QApplication::exit(0);
  }
private:
  QString fn;
};

void PrinterWE::toMultiPagePdf(QString fn) {
  Receiver *recv = new Receiver(fn);
  src->page()->printToPdf(*recv, QPageLayout(QPageSize(QPageSize::Letter), QPageLayout::Portrait, QMarginsF()));
}

void PrinterWE::toSinglePagePdf(QString fn) {
  toMultiPagePdf(fn);
  // single-page pdf not supported in WebEngine.
}

void PrinterWE::toSvg(QString fn) {
  QSvgGenerator printer;
  printer.setFileName(fn);
  printer.setResolution(90);
  QFont font;
  QFontMetricsF fm(font);
  QString txt = src->page()->title();
  QRectF r = fm.boundingRect(txt);
  printer.setSize((r.size() * 90./72).toSize());
  QPainter p;
  p.begin(&printer);
  p.setFont(font);
  p.drawText(QRectF(QPointF(0,0), r.size()), txt);
  p.end();
}

void PrinterWE::toImg(QString fn) {
  QFont font;
  QFontMetricsF fm(font);
  QString txt = src->page()->title();
  QRectF r = fm.boundingRect(txt);
  QPixmap printer(r.size().toSize());
  printer.fill(QColor("white"));
  QPainter p;
  p.begin(&printer);
  p.setFont(font);
  p.drawText(QRectF(QPointF(0,0), r.size()), txt);
  p.end();
  if (!printer.save(fn)) {
      qDebug() << "Failed to save image";
      QApplication::exit(2);
  }
 }

void PrinterWE::featureReq(QUrl const &url, QWebEnginePage::Feature f) {
    src->page()->setFeaturePermission(url, f, QWebEnginePage::PermissionDeniedByUser);
}

