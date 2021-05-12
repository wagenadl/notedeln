// Items/FontVariants.h - This file is part of NotedELN

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

// FontVariants.h

#ifndef FONTVARIANTS_H

#define FONTVARIANTS_H

#include <QFont>
#include <QFontMetricsF>
#include "MarkupStyles.h"

class FontVariants {
public:
  FontVariants(QFont const &);
  FontVariants();
  ~FontVariants();
  void setBase(QFont const &);
  QFont const *font(MarkupStyles);
  QFontMetricsF const *metrics(MarkupStyles);
private:
  static QFont *italicVersion(QFont const *f);
  static QFont *boldVersion(QFont const *f);
  static QFont *scriptVersion(QFont const *f);
private:
  QMap<MarkupStyles, QFont *> fmap;
  QMap<MarkupStyles, QFontMetricsF *> fmmap;
};

#endif
