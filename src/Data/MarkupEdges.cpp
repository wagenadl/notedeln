// MarkupEdges.cpp

#include "MarkupEdges.h"
#include <QMap>
#include <QSet>
#include <QList>
#include <QDebug>

MarkupEdges::MarkupEdges(QList<MarkupData *> const &mdd,
                         QList<TransientMarkup> const &trans) {
  QMap<int, MarkupStyles> starts;
  QMap<int, MarkupStyles> ends;
  QSet<int> all;
  foreach (MarkupData *md, mdd) {
    if (md->end()>md->start()) {
      starts[md->start()].add(md->style());
      ends[md->end()].add(md->style());
      all << md->start() << md->end();
    }
  }
  foreach (TransientMarkup const &tm, trans) {
    if (tm.end()>tm.start()) {
      starts[tm.start()].add(tm.style());
      ends[tm.end()].add(tm.style());
      all << tm.start() << tm.end();
    }
  }

  MarkupStyles st;
  QList<int> alls = all.toList();
  qSort(alls);
  foreach (int k, alls) {
    if (ends.contains(k)) {
      st.remove(ends[k]);
    }
    if (starts.contains(k)) {
      st.add(starts[k]);
    }      
    insert(k, st);
  }
  if (!contains(0))
    insert(0, MarkupStyles());
}
