// WordSet.H - This file is part of eln

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

#ifndef WORDSET_H

#define WORDSET_H

#include <QMap>
#include <QString>
#include <QPointer>
#include <QObject>
#include <QSet>
#include <QStringList>

class WordSet: public QObject {
  Q_OBJECT;
public:
  /* A WordSet collects all the words from a Data tree. Since it is relatively
     expensive to recollect continuously, WordSet responds to mod() signals
     from the tree by flagging itself "out of date". Then, it automatically
     updates itself when its contents are being probed.
   */
  WordSet(QObject *parent=0);
  virtual ~WordSet();
  void clear();
  void add(class Data const *data); // light version: does not attach
  void attach(class Data /*const*/ *data);
  void detach();
  int count(QString w);
  QSet<QString> toSet();
  QSet<QString> const &originalSet() const;
  QStringList toList();
  bool outOfDate() const;
private:
   void refresh();
private slots:
  void datamod();
private:
  QMap<QString, int> words;
  QSet<QString> origs;
  QPointer<Data> src;
  bool outofdate;
};

#endif
