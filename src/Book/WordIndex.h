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
#include <QDateTime>
#include <QVariant>

class WordIndex: public QObject {
  Q_OBJECT;
public:
  WordIndex(QObject *parent=0);
  virtual ~WordIndex();
  bool load(QString filename);
  bool save(QString filename);
  bool build(class TOC *toc, QString pagesDir);
  /* Returns true unless canceled by user. */
  void rebuildEntry(int startPage, QSet<QString> newset,
                    QSet<QString> *oldset=0);
  void dropEntry(int startPage);
  QSet<int> findWord(QString word);
  QSet<int> findPartialWord(QString wordbit); // must match at beginning of word
  QSet<int> findWords(QStringList words, bool lastPartial=false);
  /* Returned integers are start pages of entries */
  QDateTime lastSeen(int pg) const;
  bool update(class TOC const *, QString pgdir); // true if changed
private:
  void buildIndex(QVariantMap const &idx);
private:
  QMap< QString, QSet<int> > index;
  /* Maps words to sets of start pages */
  QMap<int, QDateTime> lastseen;
};

#endif
