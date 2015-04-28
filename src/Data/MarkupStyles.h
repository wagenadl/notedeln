// MarkupStyles.h

#ifndef MARKUPSTYLES_H

#define MARKUPSTYLES_H

#include "MarkupData.h"

class MarkupStyles {
public:
  MarkupStyles(): ss(0) { }
  void add(MarkupData::Style s) {
    if (s!=MarkupData::Normal)
      ss |= 1<<(int(s)-1);
  }
  void add(MarkupStyles const &a) {
    ss |= a.ss;
  }
  void remove(MarkupData::Style s) {
    if (s!=MarkupData::Normal)
      ss &= ~(1<<(int(s)-1));
  }
  void remove(MarkupStyles const &a) {
    ss &= ~a.ss;
  }
  MarkupStyles with(MarkupData::Style s) {
    MarkupStyles a = *this;
    a.add(s);
    return a;
  }
  MarkupStyles without(MarkupData::Style s) {
    MarkupStyles a = *this;
    a.remove(s);
    return a;
  }
  MarkupStyles simplified() {
    MarkupStyles a;
    if (contains(MarkupData::Subscript) || contains(MarkupData::Superscript))
      a.add(MarkupData::Superscript);
    if (contains(MarkupData::Bold))
      a.add(MarkupData::Bold);
    if (contains(MarkupData::Italic))
      a.add(MarkupData::Italic);
    return a;
  }
  bool contains(MarkupData::Style s) const {
    return (s==MarkupData::Normal) ? true
      : (ss & (1<<(int(s)-1)));
  }
  uint hash() const { return qHash(ss); }
  bool operator<(MarkupStyles const &a) const { return ss<a.ss; }
  bool operator==(MarkupStyles const &a) const { return ss==a.ss; }
  int toInt() const { return ss; }
private:
  unsigned int ss;
};

inline uint qHash(MarkupStyles ss) { return ss.hash(); }

#endif
