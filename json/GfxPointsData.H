// GfxPointsData.H

#ifndef GFXPOINTSDATA_H

#define GFXPOINTSDATA_H

#include "GfxData.H"
#include <QList>

class GfxPointsData: public GfxData {
  Q_OBJECT;
public:
  GfxPointsData(Data *parent);
  virtual ~GfxPointsData();
  QList<double> const &xx() const;
  QList<double> const &yy() const;
  void addPoint(QPointF);
  void addPoint(double x, double y);
  void removePoint(int i);
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst);
private:
  QList<double> xx_;
  QList<double> yy_;
};

#endif
