// GfxBlockData.H

#ifndef GFXBLOCKDATA_H

#define GFXBLOCKDATA_H

#include "BlockData.H"

class GfxBlockData: public BlockData {
  Q_OBJECT
  Q_PROPERTY(double xref READ xref WRITE setXref)
  Q_PROPERTY(double yref READ yref WRITE setYref)
public:
  GfxBlockData(class PageData *parent=0);
  virtual ~GfxBlockData();
  double xref() const;
  double yref() const;
  QPointF ref() const;
  void setXref(double);
  void setYref(double);
  void setRef(QPointF);
  QList<class GfxData *> const &gfx() const;
  void addGfx(GfxData *);
  void removeGfx(GfxData *);
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
protected:
  double xref_, yref_;
  QList<GfxData *> gfx_;
};

#endif
