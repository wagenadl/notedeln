// GfxNoteData.H

#ifndef GFXTEXTDATA_H

#define GFXTEXTDATA_H

#include "GfxData.H"
#include "TextData.H"

class GfxNoteData: public GfxData {
  Q_OBJECT;
  Q_PROPERTY(double dx READ dx WRITE setDx);
  Q_PROPERTY(double dy READ dy WRITE setDy);
  Q_PROPERTY(double textWidth READ textWidth WRITE setTextWidth);
  Q_PROPERTY(int sheet READ sheet WRITE setSheet);
public:
  GfxNoteData(class Data *parent=0);
  virtual ~GfxNoteData();
  double dx() const;
  double dy() const;
  double textWidth() const;
  int sheet() const;
  void setDx(double);
  void setDy(double);
  void setTextWidth(double);
  void setSheet(int);
  TextData const *text() const;
  TextData *text();
  QPointF endPoint() const;
  QPointF delta() const;
  double lineLength() const;
  bool lineLengthIsZero() const;
  void setEndPoint(QPointF);
  void setDelta(QPointF);
  void setLineLengthToZero();
  /* endPoint is calculated from pos() and (dx,dy); setEndPoint sets (dx,dy).
  */
protected:
  void loadMore(QVariantMap const &src);
private:
  TextData *text_;
  double dx_, dy_;
  double textWidth_;
  int sheet_;
};

#endif
