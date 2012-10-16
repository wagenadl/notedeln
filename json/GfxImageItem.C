// GfxImageItem.C

#include "GfxImageItem.H"
#include "GfxItemFactory.H"
#include "GfxImageData.H"
#include <QDebug>
#include "ResourceManager.H"

GfxItemFactory::Creator<GfxImageData, GfxImageItem> c("gfximage");

GfxImageItem::GfxImageItem(GfxImageData *data, QGraphicsItem *parent):
  QGraphicsPixmapItem(parent), data(data) {
  if (!data) {
    qDebug() << "GfxImageItem constructed w/o data";
    return;
  }

  // get the image, crop it, etc.
  ResourceManager *resmgr = data->resMgr();
  if (!resmgr) {
    qDebug() << "GfxImageItem: no resource manager";
    return;
  }
  if (!image.load(resmgr->path(data->resName()))) {
    qDebug() << "GfxImageItem: image load failed";
    return;
  }
  setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
  setScale(data->scale());
  setPos(data->xy());
}

GfxImageItem::~GfxImageItem() {
}

void GfxImageItem::showCroppedAreas() {
  QPixmap pm(QPixmap::fromImage(image));
  setPixmap(pm);
  setOffset(-data->cropLeft(), -data->cropTop());
}

void GfxImageItem::hideCroppedAreas() {
  setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
  setOffset(0, 0);
}

