// GfxPalette.H

#ifndef GFXPALETTE_H

#define GFXPALETTE_H

#include "GfxMarkData.H"
#include <QGraphicsObject>

class GfxPalette: public QGraphicsObject {
  Q_OBJECT;
public:
  GfxPalette(QGraphicsItem *parent=0);
  virtual ~GfxPalette();
  QColor color() const;
  double size() const;
  GfxMarkData::Shape markShape() const;
  virtual bool letUserChoose(QGraphicsScene *scene, QPointF p0);
  // p0 must be current mouse position
  // returns false if moved out of range
  // letUserChoose() must only be called when a mouse button is down
  void setColor(QColor);
  void setSize(double);
  void setMarkShape(GfxMarkData::Shape);
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
protected:
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
protected:
  void setGridSize(int rows, int cols);
  void setCellSize(double);
  virtual void paintOne(int row, int col, QPainter *)=0;
  virtual QColor colorAt(int row, int col);
  virtual double sizeAt(int row, int col);
  virtual GfxMarkData::Shape shapeAt(int row, int col);
  bool findMe(); // set row and col to match requested color/size/shape,
  // returns false if not found, in which case row and col become undefined
protected:
  QColor c;
  double siz;
  GfxMarkData::Shape shp;
  int rows;
  int cols;
  double cellsize;
  int row;
  int col;
signals:
  void release();
};

#endif
