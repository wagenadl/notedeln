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
#include "AN_Biblio.h"
#include "AN_Pubmed.h"

#include <QDebug>

bool AutoNote::autoNote(QString tag, TextItem *dest, Style const &st) {
  QString txt;
  if (QRegExp("\\d\\d\\d\\d\\d\\d*").exactMatch(tag)) {
    // At least 5 digits makes a PMID.
     txt = AN_Pubmed(tag, st).ref();
  } else if (QRegExp("\\d\\d(\\d\\d)?-[A-Za-z][A-Za-z]?[A-Za-z]?[A-Za-z]?[1-9]?").exactMatch(tag)) {
    // This is for my own bibliography system: YY-AAAAN
    txt = AN_Biblio(tag, st).ref();
  }
  if (!txt.isEmpty()) {
    dest->insertBasicHtml(txt, dest->textCursor().position());
    return true;
  }
  return false;
}
