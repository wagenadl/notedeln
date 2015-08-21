// App/RecentBooks.cpp - This file is part of eln

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

// RecentBooks.C

#include "RecentBooks.h"
#include "Notebook.h"
#include "BookFile.h"
#include <QSettings>
#include "Assert.h"

#define MAXLISTLENGTH 20

BookInfo::BookInfo(Notebook const *nb) {
  BookData const *bd = nb->bookData();
  title = bd->title();
  author = bd->author();
  address = bd->address();
  created = bd->created();
  modified = bd->modified();
  dirname = "?";
}

BookInfo::BookInfo(QString d) {
  dirname = d;
  QDir root(dirname);
  QString bookfile = root.exists("book.eln") ? "book.eln" : "book.json";    
  BookFile *bf = BookFile::load(root.filePath(bookfile));
  if (bf) {
    BookData const *bd = bf->data();
    title = bd->title();
    author = bd->author();
    address = bd->address();
    created = bd->created();
    modified = bd->modified();
    delete bf;
  }
}

bool BookInfo::operator<(BookInfo const &o) const {
  int r = QString::compare(title, o.title, Qt::CaseInsensitive);
  if (r)
    return r<0;
  r = QString::compare(author, o.author, Qt::CaseInsensitive);
  if (r)
    return r<0;
  return created < o.created;
}


//////////////////////////////////////////////////////////////////////

QString RecentBooks::keyname(int idx, QString key) {
  return QString("book-%1/%2").arg(idx).arg(key);
}

QVariant RecentBooks::get(int idx, QString key) const {
  QString k = keyname(idx, key);
  ASSERT(s->contains(k));
  return s->value(k);
}

void RecentBooks::set(int idx, QString key, QVariant const &value) {
  s->setValue(keyname(idx, key), value);
}

RecentBooks::RecentBooks() {
  s = new QSettings("net.danielwagenaar", "eln");
  for (int i=0; s->contains(keyname(i, "title")); i++) {
    QString dirname = get(i, "dirname").toString();
    data[dirname].title = get(i, "title").toString();
    data[dirname].author = get(i, "author").toString();
    data[dirname].address = get(i, "address").toString();
    data[dirname].created = get(i, "created").toDateTime();
    data[dirname].modified = get(i, "modified").toDateTime();
    data[dirname].accessed = get(i, "accessed").toDateTime();
    data[dirname].dirname = dirname;
    revmap[dirname] = i;
  }    
}

RecentBooks::~RecentBooks() {
  delete s;
}

RecentBooks *RecentBooks::instance() {
  static RecentBooks *b(new RecentBooks());
  return b;
}

void RecentBooks::addBook(Notebook const *nb) {
  QString dirname = nb->dirPath();
  int idx = -1;
  if (revmap.contains(dirname)) {
    idx = revmap[dirname];
  } else if (revmap.size()>=MAXLISTLENGTH) {
    // remove oldest
    QList<QDateTime> tt;
    foreach (QString s, data.keys()) 
      tt.append(data[s].accessed);
    qSort(tt);
    QDateTime t0 = tt.first();
    foreach (QString s, data.keys()) {
      if (data[s].accessed<=t0) {
	idx = revmap[s];
	break;
      }
    }
    ASSERT(idx>=0); 
  } else {
    idx = revmap.size();
  }

  data[dirname] = BookInfo(nb);
  data[dirname].accessed = QDateTime::currentDateTime();

  revmap[dirname] = idx;

  set(idx, "dirname", dirname);
  set(idx, "title", data[dirname].title);
  set(idx, "author", data[dirname].author);
  set(idx, "address", data[dirname].address);
  set(idx, "created", data[dirname].created);
  set(idx, "modified", data[dirname].modified);
  set(idx, "accessed", data[dirname].accessed);
}

QStringList RecentBooks::byDate() const {
  QMultiMap<QDateTime, QString> dd;
  foreach (QString s, data.keys())
    if (QDir().exists(s))
      dd.insert(data[s].accessed, s);

  QStringList l;
  foreach (QString s, dd.values())
    l.push_front(s);
  return l;
}

bool RecentBooks::contains(QString s) const {
  return data.contains(s);
}

BookInfo const &RecentBooks::operator[](QString s) const {
  ASSERT(data.contains(s));
  return data.find(s).value(); // funky syntax need to get a const & reference
}

