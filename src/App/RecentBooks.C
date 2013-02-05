// RecentBooks.C

#include "RecentBooks.H"
#include "Notebook.H"
#include <QSettings>
#include "Assert.H"

#define MAXLISTLENGTH 20

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

  BookData const *bd = nb->bookData();

  data[dirname].title = bd->title();
  data[dirname].author = bd->author();
  data[dirname].address = bd->address();
  data[dirname].created = bd->created();
  data[dirname].modified = bd->modified();
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

