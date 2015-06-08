// ResourceMagic/AutoNote.cpp - This file is part of eln

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

// AutoNote.C


#include "AutoNote.h"
#include "TextItem.h"
#include "Style.h"
#include "ResManager.h"
#include "TextData.h"
#include "MagicBiblio.h"
#include "MagicPubmed.h"

#include <QDebug>

bool AutoNote::autoNote(QString tag, TextItem *dest, Style const &st) {
  QVariantMap autos = st["auto-notes"].toMap();
  foreach (QString k, autos.keys()) {
    if (QRegExp(k).exactMatch(tag)) {
      QString func = autos[k].toString();
      QString txt = "";
      if (func=="bib") {
	txt = MagicBiblio(tag, st).ref();
      } else if (func=="pubmed") {
	txt = MagicPubmed(tag, st).ref();
      }
      if (txt.isEmpty())
	return false;
      qDebug() << "autonote -> " << txt;
      dest->insertBasicHtml(txt, dest->textCursor().position());
      return true;
    }
  }
  return false;
}
