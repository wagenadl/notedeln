// GfxSketchData.H

#ifndef GFXSKETCHDATA_H

#define GFXSKETCHDATA_H

#include "GfxPointsData.H"
#include <QColor>

class GfxSketchData: public GfxPointsData {
  Q_OBJECT;
  Q_PROPERTY(QColor color READ color WRITE setColor);
  Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth);
public:
  GfxSketchData(Data *parent=0);
  virtual ~GfxSketchData();
  QColor color() const;
  double lineWidth() const;
  void setColor(QColor);
  void setLineWidth(double);
private:
  QColor col;
  double lw;
};

#endif
