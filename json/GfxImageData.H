// GfxImageData.H

#ifndef GFXIMAGEDATA_H

#define GFXIMAGEDATA_H

#include "GfxData.H"

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
public:
  GfxImageData(Data *parent);
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
  QRectF cropRect() const; // image pixels, calc'd from above
  void setCropRect(QRectF);

  QSizeF mapImageToBlock(QSizeF) const;
  QPointF mapImageToBlock(QPointF) const;
  QRectF mapImageToBlock(QRectF) const;
  QSizeF mapBlockToImage(QSizeF) const;
  QPointF mapBlockToImage(QPointF) const;
  QRectF mapBlockToImage(QRectF) const;
  QRectF blockRect() const;
  QRectF uncroppedBlockRect() const;
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
