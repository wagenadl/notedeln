// MarkupEdges.cpp

#include "MarkupEdges.h"
#include <QMap>
#include <QSet>
#include <QList>


MarkupEdges::MarkupEdges(QList<MarkupData *> const &mdd,
                         int selstart, int selend) {
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
  if (selstart>=0 && selend>selstart) {
    starts[selstart].add(MarkupData::Selected);
    ends[selend].add(MarkupData::Selected);
    all << selstart << selend;
  }

  MarkupStyles st;
  foreach (int k, all) {
    if (ends.contains(k))
      st.remove(ends[k]);
    if (starts.contains(k))
      st.add(starts[k]);
    insert(k, st);
  }
  if (!contains(0))
    insert(0, MarkupStyles());
}
