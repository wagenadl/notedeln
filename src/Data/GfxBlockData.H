// GfxBlockData.H

#ifndef GFXBLOCKDATA_H

#define GFXBLOCKDATA_H

#include "BlockData.H"

class GfxBlockData: public BlockData {
  Q_OBJECT
  Q_PROPERTY(double height READ height WRITE setHeight)
public:
  GfxBlockData(class PageData *parent=0);
  virtual ~GfxBlockData();
  double height() const;
  void setHeight(double);
  QList<class GfxData *> const &gfx() const;
  void addGfx(GfxData *);
  void removeGfx(GfxData *);
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
protected:
  double height_;
  QList<GfxData *> gfx_;
};

#endif
