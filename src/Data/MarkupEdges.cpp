// Data/MarkupEdges.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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
