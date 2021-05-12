// Book/SearchResult.h - This file is part of NotedELN

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

// SearchResult.h

#ifndef SEARCHRESULT_H

#define SEARCHRESULT_H

#include <QList>
#include <QString>
#include <QDateTime>

struct SearchResult {
  enum Type {
    Unknown,
    InTextBlock,
    InTableBlock,
    InGfxNote,
    InLateNote,
    InFootnote,
  };
  Type type;
  int page;
  int startPageOfEntry;
  QString phrase; // search text
  QString context; // entire text of containing object
  QString entryTitle;
  QDateTime cre, mod; // of containing object
  QString uuid; // of containing object
  QList<int> whereInContext;
};

#endif
