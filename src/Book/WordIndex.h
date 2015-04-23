// WordIndex.H - This file is part of eln

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

#ifndef WORDINDEX_H

#define WORDINDEX_H

#include <QObject>
#include <QMap>
#include <QSet>

class WordIndex: public QObject {
  Q_OBJECT;
public:
  WordIndex(QObject *parent=0);
  virtual ~WordIndex();
  bool load(QString filename);
  bool save(QString filename);
  void build(class TOC *toc, QString pagesDir);
  void rebuildEntry(int startPage, class WordSet *ws);
  void dropEntry(int startPage);
  QSet<int> findWord(QString word);
  QSet<int> findPartialWord(QString wordbit); // must match at beginning of word
  QSet<int> findWords(QStringList words, bool lastPartial=false);
  /* Returned integers are start pages of entries */
private:
  typedef QSet<int> IntSet;
  typedef QSet<QString> StringSet;
  typedef QMap<QString, IntSet> MapType;
  MapType index;
  /* Maps words to sets of start pages */
};

#endif
