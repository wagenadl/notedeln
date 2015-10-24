// App/RecentBooks.H - This file is part of eln

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

// RecentBooks.H

#ifndef RECENTBOOKS_H

#define RECENTBOOKS_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariant>

struct BookInfo {
public:
  QString title;
  QString author;
  QString address;
  QDateTime created;
  QDateTime modified;
  QDateTime accessed;
  QString dirname;
public:
  BookInfo() { }
  BookInfo(QString dirname);
  BookInfo(class Notebook const *nb);
  bool operator<(BookInfo const &o) const;
};

class RecentBooks {
public:
  ~RecentBooks();
  static RecentBooks *instance();
  void addBook(class Notebook const *nb);
  QStringList byDate() const; // list of dirnames, most recently accessed first
  BookInfo const &operator[](QString dirname) const;
  bool contains(QString dirname) const;
private:
  RecentBooks(); // this constructor is private; use instance() instead.
  QVariant get(int idx, QString key) const;
  void set(int idx, QString key, QVariant const &value);
  static QString keyname(int idx, QString key);
private:
  QMap<QString, BookInfo> data; // map filename to info
  QMap<QString, int> revmap; // map filename to index in settings file
  class QSettings *s;
};

#endif
