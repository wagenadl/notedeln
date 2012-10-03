// GfxBlockData.H

#ifndef GFXBLOCKDATA_H

#define GFXBLOCKDATA_H

#include "BlockData.H"

class GfxBlockData: public BlockData {
  Q_OBJECT
public:
  GfxBlockData(class PageData *parent);
  virtual ~GfxBlockData();
  QList<class GfxData *> const &gfx() const;
  void addGfx(GfxData *);
  void removeGfx(GfxData *);
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst);
protected:
  QList<GfxData *> gfx_;
};

#endif
