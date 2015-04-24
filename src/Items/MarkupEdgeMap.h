// MarkupEdgeMap.h

#ifndef MARKUPEDGEMAP_H

#define MARKUPEDGEMAP_H

#include "MarkupData.h"
#include <QMap>

class MarkupEdgeMap {
public:
  MarkupEdgeMap(QList<MarkupData *> lst);
  QMap<int, MarkupData::Styles> const &edges();
private:
  QMap<int, MarkupData::Styles> edge2styles;
};

#endif
