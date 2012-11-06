// GfxLinePalette.H

#ifndef GFXLINEPALETTE_H

#define GFXLINEPALETTE_H

#include "GfxPalette.H"

class GfxLinePalette: public GfxPalette {
  Q_OBJECT;
public:
  GfxLinePalette(QGraphicsItem *parent=0);
  virtual ~GfxLinePalette();
protected:
  virtual QColor colorAt(int row, int col);
  virtual double sizeAt(int row, int col);
  virtual void paintOne(int row, int col, QPainter *);
};

#endif
