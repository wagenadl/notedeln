// GfxBlockItem.H

#ifndef GFXBLOCKITEM_H

#define GFXBLOCKITEM_H

#include "BlockItem.H"

class GfxBlockItem: public BlockItem {
  Q_OBJECT;
public:
  GfxBlockItem(class GfxBlockData *data, class PageScene *parent);
  virtual ~GfxBlockItem();
  GfxBlockData *data(); // benevolently hides inherited version
  QList<class GfxItem *> const &gfx();
  bool isEmpty() const;
  void newImage();
private:
  GfxBlockData *data_; // we don't own
  QList<GfxItem *> items_; // we do own
};

#endif
