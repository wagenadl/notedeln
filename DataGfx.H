// DataGfx.H

#ifndef DATAGFX_H

#define DATAGFX_H

#include "Data.H"

class DataGfx: public Data {
public:
  DataGfx(QDomElement defn);
  DataGfx(QString tag, QDomElement parent, QString type="gfx");
  double x() const;
  double y() const;
  void setX(double x);
  void setY(double y);
};

#endif
