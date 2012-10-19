// GfxNoteData.H

#ifndef GFXTEXTDATA_H

#define GFXTEXTDATA_H

#include "GfxData.H"
#include "TextData.H"

class GfxNoteData: public GfxData {
  Q_OBJECT;
  Q_PROPERTY(HAlign hAlign READ hAlign WRITE setHAlign);
  Q_PROPERTY(VAlign vAlign READ vAlign WRITE setVAlign);
  Q_PROPERTY(double dx READ dx WRITE setDx);
  Q_PROPERTY(double dy READ dy WRITE setDy);
  Q_PROPERTY(double textWidth READ textWidth WRITE setTextWidth);
  Q_ENUMS(HAlign VAlign);
public:
  enum HAlign {
    Left,
    Right,
    Center,
  };
  enum VAlign {
    Top,
    Bottom,
    Middle,
    Baseline,
  };
public:
  GfxNoteData(class Data *parent=0);
  virtual ~GfxNoteData();
  HAlign hAlign() const;
  VAlign vAlign() const;
  double dx() const;
  double dy() const;
  double textWidth() const;
  void setHAlign(HAlign);
  void setVAlign(VAlign);
  void setDx(double);
  void setDy(double);
  void setTextWidth(double);
  TextData const *text() const;
  TextData *text();
  QPointF textPos() const;
  void setTextPos(QPointF, bool realign=false);
  /* textPos is calculated from pos() and (dx,dy); setTextPos sets (dx,dy).
     If realign=true, alignment is reset based on (dx,dy). */
protected:
  void loadMore(QVariantMap const &src);
private:
  HAlign hAlign_;
  VAlign vAlign_;
  TextData *text_;
  double dx_, dy_;
  double textWidth_;
};

#endif
