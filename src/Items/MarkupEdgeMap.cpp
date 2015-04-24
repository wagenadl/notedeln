// MarkupEdgeMap.cpp

#include "MarkupEdgeMap.h"
#include <QList>
#include "MarkupData.h"

MarkupEdgeMap::MarkupEdgeMap(QList<MarkupData *> markups) {
  foreach (MarkupData *m, markups) {
    edge2styles[m->start()] |= m->style();
    edge2styles[m->end()] |= MarkupData::Normal;
  }
  // OK. But now we may still have styles that remain active across
  // multiple edges.
  // None of this is terribly efficient, but it may be good enough
  QList<int> edges = edge2styles.keys();
  foreach (MarkupData *m, markups) 
    foreach (int edge, edges) 
      if (m->start()<edge && m->end()>edge)
        edge2styles[edge] |= m->style();
}  

QMap<int, MarkupData::Styles> const &MarkupEdgeMap::edges() {
  return edge2styles;
}
