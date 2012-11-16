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
  void paginate(bool);
public slots:
  void complete(bool ok);
private:
  QGraphicsWebView *src;
  QString ofn;
  bool paginate_;
};

#endif
