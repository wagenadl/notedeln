// MarkupEdges.h

#ifndef MARKUPEDGES_H

#define MARKUPEDGES_H

#include <QMap>
#include "MarkupStyles.h"

class MarkupEdges: public QMap<int, MarkupStyles> {
public:
  MarkupEdges(QList<MarkupData *> const &, int selstart=-1, int selend=-1);
};

#endif
