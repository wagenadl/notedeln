// GfxImageItem.H

#ifndef GFXIMAGEITEM_H

#define GFXIMAGEITEM_H

#include <QGraphicsPixmapItem>

class GfxImageItem: public QGraphicsPixmapItem {
public:
  GfxImageItem(class GfxImageData *data, QGraphicsItem *parent=0);
  virtual ~GfxImageItem();
  void showCroppedAreas();
  void hideCroppedAreas();
  // whatever magic to crop and resize?
private:
  GfxImageData *data;
  QImage image; // not cropped
};

#endif
