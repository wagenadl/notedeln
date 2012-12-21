// Linewidthbar.H

#ifndef LINEWIDTHBAR_H

#define LINEWIDTHBAR_H

#include "Toolbar.H"

class LineWidthBar: public Toolbar {
  Q_OBJECT;
public:
  LineWidthBar(class Mode *mode, QGraphicsItem *parent);
  virtual ~LineWidthBar();
public slots:
  void setColor(QColor);
protected:
  virtual void doLeftClick(QString id);
private:
  static double idToWidth(QString);
  static QString widthToId(double);
private:
  Mode *mode;
};

#endif
