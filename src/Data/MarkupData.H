// MarkupData.H

#ifndef MARKUPDATA_H

#define MARKUPDATA_H

#include "Data.H"

class MarkupData: public Data {
  /* This is for simple posthoc annotations of text as well as italics,
     scripts, hyperlinks. */
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
    CustomRef,
    Superscript,
    Subscript,
    StrikeThrough,
    Emphasize,
  };
public:
  MarkupData(Data *parent=0);
  MarkupData(int start, int end, Style style, Data *parent=0);
  virtual ~MarkupData();
  // read properties
  int start() const;
  int end() const;
  Style style() const;
  // write properties
  void setStart(int);
  void setEnd(int);
  void setStyle(Style);
  // other
  bool operator<(MarkupData const &other) const;
  void merge(MarkupData const *other);
  bool update(int pos, int del, int ins);
private:
  bool cut(int pos, int len);
  bool insert(int pos, int len);
private:
  int start_;
  int end_;
  Style style_;
};

#endif
