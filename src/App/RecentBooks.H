// RecentBooks.H

#ifndef RECENTBOOKS_H

#define RECENTBOOKS_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QDateTime>

struct BookInfo {
  QString title;
  QString author;
  QString address;
  QDateTime created;
  QDateTime modified;
  QDateTime accessed;
};

class RecentBooks {
public:
  ~RecentBooks();
  static RecentBooks *instance();
  void addBook(class Notebook const *nb);
  QStringList byDate() const; // list of dirnames, most recently accessed first
  BookInfo const &operator[](QString) const;
  bool contains(QString) const;
private:
  RecentBooks(); // this constructor is private; use instance() instead.
  QVariant get(int idx, QString key) const;
  void set(int idx, QString key, QVariant const &value);
  static QString keyname(int idx, QString key);
private:
  QMap<QString, BookInfo> data;
  QMap<QString, int> revmap; // map to file keys
  class QSettings *s;
};

#endif
