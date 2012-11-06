// GfxMarkPalette.H

#ifndef GFXMARKPALETTE_H

#define GFXMARKPALETTE_H

#include "GfxPalette.H"

class GfxMarkPalette: public GfxPalette {
  Q_OBJECT;
public:
  GfxMarkPalette(QGraphicsItem *parent=0);
  virtual ~GfxMarkPalette();
protected:
  virtual double sizeAt(int row, int col);
  virtual GfxMarkData::Shape shapeAt(int row, int col);
  virtual void paintOne(int row, int col, QPainter *);
};

#endif
