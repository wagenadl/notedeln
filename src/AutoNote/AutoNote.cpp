// ResourceMagic/AutoNote.cpp - This file is part of NotedELN

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
  if (QRegularExpression("\\d\\d\\d\\d\\d\\d*").match(tag).hasMatch()) {
    // At least 5 digits makes a PMID.
     txt = AN_Pubmed(tag, st).ref();
  } else if (QRegularExpression("\\d\\d(\\d\\d)?-[A-Za-z][A-Za-z]?[A-Za-z]?[A-Za-z]?[1-9]?").match(tag).hasMatch()) {
    // This is for my own bibliography system: YY-AAAAN
    txt = AN_Biblio(tag, st).ref();
  }
  qDebug() << "AutoNote" << tag << txt;
  if (!txt.isEmpty()) {
    dest->insertBasicHtml(txt, dest->textCursor().position());
    return true;
  }
  return false;
}
