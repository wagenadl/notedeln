// GfxMarkData.H

#ifndef GFXMARKDATA_H

#define GFXMARKDATA_H

#include "GfxData.H"
#include <QColor>

class GfxMarkData: public GfxData {
  Q_OBJECT;
  Q_ENUMS(Shape)
  Q_PROPERTY(QColor color READ color WRITE setColor);
  Q_PROPERTY(double size READ size WRITE setSize);
  Q_PROPERTY(Shape shape READ shape WRITE setShape);
public:
  enum Shape {
    SolidCircle,
    SolidSquare,
    Cross,
    Plus,
    OpenCircle,
    OpenSquare,
    DotCircle,
    DotSquare,
  };
public:
  GfxMarkData(Data *parent=0);
  virtual ~GfxMarkData();
  QColor color() const;
  double size() const;
  Shape shape() const;
  void setColor(QColor);
  void setSize(double);
  void setShape(Shape);
private:
  QColor col;
  double siz;
  Shape shp;
};

#endif
