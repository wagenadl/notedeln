// GfxDoodleData.H

#ifndef GFXDOODLEDATA_H

#define GFXDOODLEDATA_H

#include "GfxPointsData.H"
#include <QColor>

class GfxDoodleData: public GfxPointsData {
  Q_OBJECT;
  Q_PROPERTY(QColor color READ color WRITE setColor);
  Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth);
public:
  GfxDoodleData(Data *parent=0);
  virtual ~GfxDoodleData();
  QColor color() const;
  double lineWidth() const;
  void setColor(QColor);
  void setLineWidth(double);
private:
  QColor col;
  double lw;
};

#endif
