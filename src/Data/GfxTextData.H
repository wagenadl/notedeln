// GfxTextData.H

#ifndef GFXTEXTDATA_H

#define GFXTEXTDATA_H

#include "GfxData.H"
#include "MarkupData.H"

class GfxTextData: public GfxData {
  Q_OBJECT;
  Q_PROPERTY(HAlign hAlign READ hAlign WRITE setHAlign);
  Q_PROPERTY(VAlign vAlign READ vAlign WRITE setVAlign);
  Q_PROPERTY(QString text READ text WRITE setText)
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
  QString text() const;
  void setHAlign(HAlign);
  void setVAlign(VAlign);
  void setText(QString const &);
  QList<MarkupData *> markups() const;
  MarkupData *addMarkup(int start, int end, MarkupData::Style style);
  void deleteMarkup(MarkupData *);
private:
  HAlign hAlign_;
  VAlign vAlign_;
  QString text_;
};

#endif
