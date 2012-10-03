// GfxPointsData.H

#ifndef GFXPOINTSDATA_H

#define GFXPOINTSDATA_H

#include "GfxData.H"

class GfxPointsData: public GfxData {
  Q_OBJECT;
  Q_PROPERTY(QList xx READ xx WRITE setXX)
  Q_PROPERTY(QList yy READ yy WRITE setYY)
  /* the xx and yy coordinates are relative to the x and y coordinates of
     the GfxData. */
public:
  GfxPointsData(class GfxBlockData *parent);
  virtual ~GfxPointsData();
  QList<double> const &xx() const;
  QList<double> const &yy() const;
  void addPoint(QPointF);
  void addPoint(double x, double y);
  void removePoint(int i);
private: // only for load() - does this work?
  void setXX(QList<double> const &);
  void setYY(QList<double> const &);
private:
  QList<double> xx_;
  QList<double> yy_;
};

#endif
