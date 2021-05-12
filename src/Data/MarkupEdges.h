// Data/MarkupEdges.h - This file is part of NotedELN

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
