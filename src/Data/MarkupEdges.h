// MarkupEdges.h

#ifndef MARKUPEDGES_H

#define MARKUPEDGES_H

#include <QMap>
#include "MarkupStyles.h"

class TransientMarkup {
public:
  TransientMarkup(int start=0, int end=0,
                  MarkupData::Style style=MarkupData::Normal):
    st(start), en(end), sty(style) { }
  int start() const { return st; }
  int end() const { return en; }
  MarkupData::Style style() const { return sty; }
private:
  int st, en;
  MarkupData::Style sty;
};

class MarkupEdges: public QMap<int, MarkupStyles> {
public:
  MarkupEdges(QList<MarkupData *> const &mdd,
              QList<TransientMarkup> const &trans=QList<TransientMarkup>());
};

#endif
