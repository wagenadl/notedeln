// GfxTextData.H

#ifndef GFXTEXTDATA_H

#define GFXTEXTDATA_H

#include "GfxData.H"
#include "TextData.H"

class GfxTextData: public GfxData {
  Q_OBJECT;
  Q_PROPERTY(HAlign hAlign READ hAlign WRITE setHAlign);
  Q_PROPERTY(VAlign vAlign READ vAlign WRITE setVAlign);
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
  GfxTextData(class Data *parent);
  virtual ~GfxTextData();
  HAlign hAlign() const;
  VAlign vAlign() const;
  void setHAlign(HAlign);
  void setVAlign(VAlign);
  TextData *text() const;
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
private:
  HAlign hAlign_;
  VAlign vAlign_;
  TextData *text_;
};

#endif
