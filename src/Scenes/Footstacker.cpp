// Scenes/Footstacker.cpp - This file is part of eln

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

// Footstacker.cpp

#include "Footstacker.h"
#include <QMultiMap>

Footstacker::Footstacker(BlockItem *bi) {
  QMultiMap<double, FootnoteItem *> foots;
  foreach (FootnoteItem *fni, bi->footnotes()) {
    FootnoteData *fnd = fni->data();
    QString tag = fnd->tag();
    QPointF p = bi->findRefText(tag);
    foots.insert(p.y() + 0.001*p.x(), fni);
  }

  /* Now we know where each of the footnotes wants to be attached, and
     we know their heights. But it is not in a convenient shape. Let's
     reorganize.
   */
  double cumh = 0;
  cumulHBefore << cumh;
  for (QMultiMap<double, FootnoteItem *>::iterator i=foots.begin();
       i!=foots.end(); ++i) {
    attach << i.key();
    notes << i.value();
    double h = i.value()->data()->height();
    height << h;
    cumh += h;
    cumulHBefore << cumh;
  }
}

int Footstacker::nBefore(double y) {
  int n = 0;
  foreach (double a, attach) {
    if (a<y)
      n++;
    else
      break;
  }
  return n;
}
