// TOC.H

#ifndef TOC_H

#define TOC_H

#include <QObject>
#include <QVariant>
#include <QDate>

class TOCEntry {
public:
  QString title;
  QDate created;
  int sheetCount;
public:
  TOCEntry(QString title="",
	   QDate created=QDate::currentDate(),
	   int sheetCount=1);
  QVariantMap save() const;
  void load(QVariantMap const &);
};

class TOC: public QObject {
  Q_OBJECT;
public:
  TOC(QString fn, QObject *parent=0); // loads fn
  virtual ~TOC();
  QMap<int, TOCEntry> const &entries() const;
  TOCEntry const &entry(int pgno) const; // null Entry if none
  bool contains(int startPage) const;
  void addEntry(int startPage, TOCEntry const &entry);
  int nextPage() const;
  bool ok() const; // true if last save ok
public slots:
  void setTitle(int startPage, QString title);
  void setSheetCount(int startPage, int sheetCount);
  void save() const;
public:
  static TOC *create(QString fn, QObject *parent=0);
private:
  TOC(QObject *parent=0); // doesn't load
private:
  QMap<int, TOCEntry> entries_;
  QString fn;
  mutable bool ok_;
};

#endif
