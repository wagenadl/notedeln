// GfxImageData.C

#include "GfxImageData.H"
#include <QRectF>
#include <QDebug>

static Data::Creator<GfxImageData> c("gfximage");

GfxImageData::GfxImageData(Data *parent): GfxData(parent) {
}

GfxImageData::~GfxImageData() {
}

QString GfxImageData::resName() const {
  return resName_;
}

double GfxImageData::width() const {
  return width_;
}

double GfxImageData::height() const {
  return height_;
}

double GfxImageData::scale() const {
  return scale_;
}

double GfxImageData::cropTop() const {
  return cropTop_;
}

double GfxImageData::cropBottom() const {
  return cropBottom_;
}

double GfxImageData::cropLeft() const {
  return cropLeft_;
}

double GfxImageData::cropRight() const {
  return cropRight_;
}

void GfxImageData::setResName(QString s) {
  resName_ = s;
  markModified();
}

void GfxImageData::setWidth(double w) {
  width_ = w;
  markModified();
}
  
void GfxImageData::setHeight(double h) {
  height_ = h;
  markModified();
}
  
void GfxImageData::setScale(double s) {
  scale_ = s;
  markModified();
}

void GfxImageData::setCropTop(double v) {
  cropTop_ = v;
}

void GfxImageData::setCropBottom(double v) {
  cropBottom_ = v;
}
  
void GfxImageData::setCropLeft(double v) {
  cropLeft_ = v;
}

void GfxImageData::setCropRight(double v) {
  cropRight_ = v;
}

QRectF GfxImageData::cropRect() const {
  return QRectF(cropLeft_,
		cropTop_,
		width_ - cropLeft_- cropRight_,
		height_ - cropTop_ - cropBottom_);
}

void GfxImageData::setCropRect(QRectF r) {
  cropLeft_ = r.left();
  cropRight_ = width_ - r.right();
  cropTop_ = r.top();
  cropBottom_ = height_ - r.bottom();
}

QSizeF GfxImageData::mapImageToBlock(QSizeF s) const {
  return s*scale_;
}

QSizeF GfxImageData::mapBlockToImage(QSizeF s) const {
  return s/scale_;
}

QPointF GfxImageData::mapImageToBlock(QPointF p) const {
  return (p-cropRect().topLeft()) * scale_ + xy();
}

QPointF GfxImageData::mapBlockToImage(QPointF p) const {
  return (p - xy()) / scale_ + cropRect().topLeft();
}

QRectF GfxImageData::mapImageToBlock(QRectF r) const {
  return QRectF(mapImageToBlock(r.topLeft()), mapImageToBlock(r.size()));
}

QRectF GfxImageData::mapBlockToImage(QRectF r) const {
  return QRectF(mapBlockToImage(r.topLeft()), mapBlockToImage(r.size()));
}

QRectF GfxImageData::blockRect() const {
  return mapImageToBlock(cropRect());
}

QRectF GfxImageData::uncroppedBlockRect() const {
  return mapImageToBlock(QRectF(0, 0, width_, height_));
}
