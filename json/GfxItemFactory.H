// GfxItemFactory.H

#ifndef GFXITEMFACTORY_H

#define GFXITEMFACTORY_H

#include <QMap>
#include <QGraphicsItem>
#include "GfxBlockItem.H"

class GfxData;
class GfxBlockItem;

class GfxItemFactory {
public:
  static QGraphicsItem *create(GfxData *data, GfxBlockItem *parent);
  static QMap<QString, QGraphicsItem *(*)(GfxData *, GfxBlockItem *)>
    &creators();
  template <class DT, class IT> class Creator {
  public:
    Creator<DT, IT>(QString typ) {
      GfxItemFactory::creators()[typ] = &create;
    }
    static QGraphicsItem *create(GfxData *data, GfxBlockItem *parent) {
      return new IT(dynamic_cast<DT*>(data), parent);
    }
  };
};

#endif
