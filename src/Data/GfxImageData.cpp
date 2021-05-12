// Data/GfxImageData.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// GfxImageData.C

#include "GfxImageData.h"
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
  attachResource(resName);
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
  if (resName_==s)
    return;
  resName_ = s;
  markModified();
}

void GfxImageData::setWidth(double w) {
  if (width_==w)
    return;
  width_ = w;
  markModified();
}
  
void GfxImageData::setHeight(double h) {
  if (height_==h)
    return;
  height_ = h;
  markModified();
}
  
void GfxImageData::setScale(double s) {
  if (scale_==s)
    return;
  scale_ = s;
  markModified();
}

void GfxImageData::setCropTop(double v) {
  if (cropTop_==v)
    return;
  cropTop_ = v;
  markModified();
}

void GfxImageData::setCropBottom(double v) {
  if (cropBottom_==v)
    return;
  cropBottom_ = v;
  markModified();
}
  
void GfxImageData::setCropLeft(double v) {
  if (cropLeft_==v)
    return;
  cropLeft_ = v;
  markModified();
}

void GfxImageData::setCropRight(double v) {
  if (cropRight_==v)
    return;
  cropRight_ = v;
  markModified();
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
  if (width_==s.width() && height_==s.height())
    return;
  width_ = s.width();
  height_ = s.height();
  markModified();
}

void GfxImageData::setCropRect(QRectF r) {
  if (cropLeft_ == r.left()
      && cropRight_ == width_ - r.right()
      && cropTop_ == r.top()
      && cropBottom_ == height_ - r.bottom())
    return;
  cropLeft_ = r.left();
  cropRight_ = width_ - r.right();
  cropTop_ = r.top();
  cropBottom_ = height_ - r.bottom();
  markModified();
}
