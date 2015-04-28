// ResourceMagic/DWBibMagician.cpp - This file is part of eln

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

// DWBibMagician.C

#include "DWBibMagician.h"
#include "Style.h"
#include "MagicBiblio.h"
#include <QDebug>

DWBibMagician::DWBibMagician(): style(Style::defaultStyle()) {
}

DWBibMagician::DWBibMagician(Style const &style): style(style) {
}

bool DWBibMagician::matches(QString s) const {
  MagicBiblio b(s, style);
  bool res = b.ok();
  if (res)
    qDebug() << "DWBibMagician matches " << s;
  return b.ok();
}

QUrl DWBibMagician::objectUrl(QString s) const {
  MagicBiblio b(s, style);
  return b.ok() ? b.url() : QUrl();
}

QString DWBibMagician::title(QString s) const {
  MagicBiblio b(s, style);
  return b.ref();
}


//bool DWBibMagician::keepAlways(QString) const {
//  return true;
//}
