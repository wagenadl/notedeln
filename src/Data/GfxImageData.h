// Data/GfxImageData.H - This file is part of NotedELN

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

// GfxImageData.H

#ifndef GFXIMAGEDATA_H

#define GFXIMAGEDATA_H

#include "GfxData.h"

class GfxImageData: public GfxData {
  Q_OBJECT;
  Q_PROPERTY(QString resName READ resName WRITE setResName)
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(double height READ height WRITE setHeight)
  Q_PROPERTY(double scale READ scale WRITE setScale)
  Q_PROPERTY(double cropTop READ cropTop WRITE setCropTop)
  Q_PROPERTY(double cropBottom READ cropBottom WRITE setCropBottom)
  Q_PROPERTY(double cropLeft READ cropLeft WRITE setCropLeft)
  Q_PROPERTY(double cropRight READ cropRight WRITE setCropRight)
  /* The pos() is the block position of the topleft of the uncropped image.
     cropXXX() are in pixels in the image, all measured from top left.
     scale() is page units per image pixel
     width() and height() are in pixels.
  */
public:
  GfxImageData(Data *parent=0);
  GfxImageData(QString resName, QImage const &img, Data *parent=0);
  virtual ~GfxImageData();
public:
  // read properties
  QString resName() const;
  double width() const;  // image pixels
  double height() const; // image pixels
  double scale() const; // page units per image pixel; we only support
                        // preserved aspect ratio
  double cropTop() const; // image pixels
  double cropBottom() const;
  double cropLeft() const;
  double cropRight() const;
  // write properties
  void setResName(QString);
  void setWidth(double);
  void setHeight(double);
  void setScale(double);
  void setCropTop(double);
  void setCropBottom(double);
  void setCropLeft(double);
  void setCropRight(double);
  // other
  QSizeF size() const; // image pixels
  QRectF cropRect() const; // image pixels, calc'd from above
  void setSize(QSizeF);
  void setCropRect(QRectF);

  // QSizeF mapToBlock(QSizeF) const;
  // QPointF mapToBlock(QPointF) const;
  // QRectF mapToBlock(QRectF) const;
  // QSizeF mapFromBlock(QSizeF) const;
  // QPointF mapFromBlock(QPointF) const;
  // QRectF mapFromBlock(QRectF) const;
  // QRectF blockRect() const;
  // QRectF uncroppedBlockRect() const;
private:
  QString resName_;
  double width_;
  double height_;
  double scale_;
  double cropTop_;
  double cropBottom_;
  double cropLeft_;
  double cropRight_;
};

#endif
