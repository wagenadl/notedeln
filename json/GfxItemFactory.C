// GfxItemFactory.C

#include "GfxItemFactory.H"
#include "GfxData.H"
#include "GfxBlockItem.H"

QGraphicsItem *GfxItemFactory::create(GfxData *data, GfxBlockItem *parent) {
  QString typ = data->type();
  if (creators().contains(typ))
    return creators()[typ](data, parent);
  else
    return 0;
}

QMap<QString, QGraphicsItem *(*)(GfxData *, GfxBlockItem *)>
  &GfxItemFactory::creators() {
  static QMap<QString, QGraphicsItem *(*)(GfxData *, GfxBlockItem *)> m;
  return m;
}
  
