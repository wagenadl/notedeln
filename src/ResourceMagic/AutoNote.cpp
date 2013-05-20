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


#include "AutoNote.H"
#include "TextItem.H"
#include "Style.H"
#include "ResManager.H"
#include "TextData.H"
#include "MagicBiblio.H"

#include <QDebug>

static QString autoNote(QString funcName, QString tag, Style const &st) {
  qDebug() << "AutoNote" << funcName << tag;
  if (funcName=="bib")
    return MagicBiblio(tag, st).ref();
  else
    return "";
}

bool AutoNote::autoNote(QString tag, TextItem *dest, Style const &st) {
  QVariantMap autos = st["auto-notes"].toMap();
  foreach (QString k, autos.keys()) {
    QRegExp re(k);
    if (re.exactMatch(tag)) {
      QString txt = ::autoNote(autos[k].toString(), tag, st);
      if (txt.isEmpty())
	return false;
      qDebug() << "autonote -> " << txt;
      dest->insertBasicHtml(txt, dest->textCursor().position());
      return true;
    }
  }
  return false;
}
