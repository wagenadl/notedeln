// webgrab/Printer.H - This file is part of eln

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

#ifndef PRINTER_H

#define PRINTER_H

#include <QObject>
#include <QSizeF>

#include "Options.h"

class Printer: public QObject {
  Q_OBJECT;
public:
  Printer(class QGraphicsWebView *src, Options const &opt);
  virtual ~Printer();
public slots:
  void complete(bool ok);
private:
  void toPdf(QString);
  void toSinglePagePdf(QString);
  void toMultiPagePdf(QString);
  void toSvg(QString);
  void toImg(QString);
  void display();
private:
  class QGraphicsScene *s;
  class QGraphicsView *v;
  QGraphicsWebView *src;
  Options const &opt;
};

#endif
