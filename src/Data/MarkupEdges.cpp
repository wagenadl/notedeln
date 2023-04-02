// Data/MarkupEdges.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// MarkupEdges.cpp

#include "MarkupEdges.h"
#include <QMap>
#include <QSet>
#include <QList>
#include <QDebug>
#include "Style.h"

MarkupEdges::MarkupEdges(QList<MarkupData *> const &mdd,
                         QDateTime parentcre,
                         QList<TransientMarkup> const &trans) {
  QMap<int, MarkupStyles> starts;
  QMap<int, MarkupStyles> ends;
  QSet<int> all;
  QDateTime parentcre_nextmorning(parentcre.date().addDays(1), QTime(4,0,0));
  if (mdd.size())
    parentcre_nextmorning
      .setTime(QTime(((Data*)mdd[0])->style().real("midnight-allowance"),
                     0, 0));
  foreach (MarkupData *md, mdd) {
    if (md->end()>md->start()) {
      QDateTime mdcre = md->created();
      MarkupData::Style sty = md->style();
      if (mdcre > parentcre_nextmorning) {
        // late
        if (sty==MarkupData::StrikeThrough)
          sty = MarkupData::LateStrikeThrough;
        else if (sty==MarkupData::Emphasize)
          sty = MarkupData::LateEmphasize;
      }
      starts[md->start()].add(sty);
      ends[md->end()].add(sty);
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
  QList<int> alls = all.values();
  std::sort(alls.begin(), alls.end());
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
