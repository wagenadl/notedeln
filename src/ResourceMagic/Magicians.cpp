// ResourceMagic/Magicians.cpp - This file is part of eln

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

// Magicians.C

#include "Magicians.H"
#include "DWBibMagician.H"
#include "WebPageMagician.H"

QMap<Style const *, Magicians *> &Magicians::stylemap() {
  static QMap<Style const *, Magicians *> map;
  return map;
}

Magicians const &Magicians::magicians(Style const &st) {
  if (stylemap().contains(&st))
    return *stylemap()[&st];
  Magicians *m1 = new Magicians(st);
  stylemap()[&st] = m1;
  return *m1;
}


Magician const *Magicians::first(QString refText) const {
  foreach (Magician const *m, mm)
    if (m->matches(refText))
      return m;
  return 0;
}

Magician const *Magicians::next(QString refText, Magician const *m0) const {
  bool gotcha = false;
  foreach (Magician const *m, mm) {
    if (gotcha)
      if (m->matches(refText))
	return m;
    if (m==m0)
      gotcha = true;
  }
  return 0;
}

// ----------------------------------------------------------------------
// Here's where we build magicians
// ----------------------------------------------------------------------
Magicians::Magicians(Style const &st) {
  mm.append(new UrlMagician());
  mm.append(new DWBibMagician(st));

  QVariantMap vm = st["magicians"].toMap();
  foreach (QVariant v, vm) {
    QVariantMap m = v.toMap();
    if (m.contains("link-key")) 
      mm.append(new WebPageLinkMagician(m));
    else
      mm.append(new SimpleMagician(m));
  }
}
