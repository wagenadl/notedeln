// GfxData.H

#ifndef GFXDATA_H

#define GFXDATA_H

#include "Data.H"
#include <QPointF>

class GfxData: public Data {
  Q_OBJECT
  Q_PROPERTY(double x READ x WRITE setX)
  Q_PROPERTY(double y READ y WRITE setY)
public:
  GfxData(Data *parent=0);
  ~GfxData();
  // read properties
  double x() const;
  double y() const;
  // write properties
  void setX(double);
  void setY(double);
  // convenience
  QPointF xy() const;
  void setXY(QPointF);
protected:
  double x_;
  double y_;
};

#endif
