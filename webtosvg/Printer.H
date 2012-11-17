// Printer.H

#ifndef PRINTER_H

#define PRINTER_H

#include <QObject>
#include <QSizeF>

class Printer: public QObject {
  Q_OBJECT;
public:
  Printer(class QGraphicsWebView *src, QString ofn);
  virtual ~Printer();
public slots:
  void complete(bool ok);
private:
  QGraphicsWebView *src;
  QString ofn;
};

#endif
