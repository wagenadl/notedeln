// Printer.H

#ifndef PRINTER_H

#define PRINTER_H

#include <QObject>
#include <QSizeF>

#include "Options.H"

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
