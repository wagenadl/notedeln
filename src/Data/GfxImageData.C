// GfxImageData.C

#include "GfxImageData.H"
#include <QRectF>
#include <QDebug>
#include <QImage>

static Data::Creator<GfxImageData> c("gfximage");

GfxImageData::GfxImageData(Data *parent): GfxData(parent) {
  setType("gfximage");
  resName_ = "";
  width_ = 1;
  height_ = 1;
  scale_ = 1;
  cropTop_ = 0;
  cropBottom_ = 0;
  cropLeft_ = 0;
  cropRight_ = 0;
}

GfxImageData::GfxImageData(QString resName, QImage img, Data *parent):
  GfxData(parent) {
  setType("gfximage");
  resName_ = resName;
  width_ = img.width();
  height_ = img.height();
  scale_ = 1;
  if (width_ * scale_ > 5*72) // arbitrary
    scale_ = 5*72/width_;
  if (height_ * scale_ > 5*72) // arbitrary
    scale_ = 5*72/height_;
  cropTop_ = 0;
  cropBottom_ = 0;
  cropLeft_ = 0;
  cropRight_ = 0;
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

QSizeF GfxImageData::size() const {
  return QSizeF(width_, height_);
}

QRectF GfxImageData::cropRect() const {
  return QRectF(cropLeft_,
		cropTop_,
		width_ - cropLeft_- cropRight_,
		height_ - cropTop_ - cropBottom_);
}

void GfxImageData::setSize(QSizeF s) {
  width_ = s.width();
  height_ = s.height();
  markModified();
}

void GfxImageData::setCropRect(QRectF r) {
  cropLeft_ = r.left();
  cropRight_ = width_ - r.right();
  cropTop_ = r.top();
  cropBottom_ = height_ - r.bottom();
  markModified();
}

QSizeF GfxImageData::mapToBlock(QSizeF s) const {
  return s*scale_;
}

QSizeF GfxImageData::mapFromBlock(QSizeF s) const {
  return s/scale_;
}

QPointF GfxImageData::mapToBlock(QPointF p) const {
  return (p-cropRect().topLeft()) * scale_ + pos();
}

QPointF GfxImageData::mapFromBlock(QPointF p) const {
  return (p - pos()) / scale_ + cropRect().topLeft();
}

QRectF GfxImageData::mapToBlock(QRectF r) const {
  return QRectF(mapToBlock(r.topLeft()), mapToBlock(r.size()));
}

QRectF GfxImageData::mapFromBlock(QRectF r) const {
  return QRectF(mapFromBlock(r.topLeft()), mapFromBlock(r.size()));
}

QRectF GfxImageData::blockRect() const {
  return mapToBlock(cropRect());
}

QRectF GfxImageData::uncroppedBlockRect() const {
  return mapToBlock(QRectF(0, 0, width_, height_));
}

