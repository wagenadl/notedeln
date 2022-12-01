// webgrab/PrinterWE.H - This file is part of eln

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

// Printer.H

#ifndef PRINTERWE_H

#define PRINTERWE_H

#include <QObject>
#include <QSizeF>
#include <QWebEnginePage>

#include "Options.h"

class PrinterWE: public QObject {
  Q_OBJECT;
public:
  PrinterWE(class QWebEngineView *src, Options const &opt);
  virtual ~PrinterWE();
public slots:
  void complete(bool);
  void featureReq(QUrl const &, QWebEnginePage::Feature);
  void sizeChange(QSizeF const &);
private:
  void toPdf(QString);
  void toSinglePagePdf(QString);
  void toMultiPagePdf(QString);
  void display();
private:
  QWebEngineView *src;
  Options const &opt;
  bool havesize;
  QString temppdf;
};

#endif
