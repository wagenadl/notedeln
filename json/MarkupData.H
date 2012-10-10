// MarkupData.H

#ifndef MARKUPDATA_H

#define MARKUPDATA_H

#include "Data.H"

class MarkupData: public Data {
  /* This is for simple posthoc annotations of text */
  Q_OBJECT;
  Q_ENUMS(Style)
  Q_PROPERTY(int start READ start WRITE setStart)
  Q_PROPERTY(int end READ end WRITE setEnd)
  Q_PROPERTY(Style style READ style WRITE setStyle)
  Q_ENUMS(Style);
public:
  enum Style {
    Normal,
    Italic,
    Bold,
    Underline,
    URL,
    Superscript,
    Subscript,
    StrikeThrough,
    Emphasize,
  };
public:
  MarkupData(class TextBlockData *parent);
  virtual ~MarkupData();
  // read properties
  int start() const;
  int end() const;
  Style style() const;
  // write properties
  void setStart(int);
  void setEnd(int);
  void setStyle(Style);
protected:
  int start_;
  int end_;
  Style style_;
};

#endif
