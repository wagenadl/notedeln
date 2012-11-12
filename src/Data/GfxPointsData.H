// GfxPointsData.H

#ifndef GFXPOINTSDATA_H

#define GFXPOINTSDATA_H

#include "GfxData.H"
#include <QList>

class GfxPointsData: public GfxData {
  Q_OBJECT;
  /* Lists xx() and yy() are relative to pos(). */
public:
  GfxPointsData(Data *parent);
  virtual ~GfxPointsData();
  QList<double> const &xx() const;
  QList<double> const &yy() const;
  QPointF point(int) const;
  int size() const;
  void setPoint(int, QPointF, bool hush=false);
  void addPoint(QPointF, bool hush=false);
  void removePoint(int i);
  void clear();
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
private:
  QList<double> xx_;
  QList<double> yy_;
};

#endif
