// TOC.H

#ifndef TOC_H

#define TOC_H

#include <QObject>
#include <QVariant>
#include <QDate>
#include "Data.H"
#include "DataFile.H"

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

class TOC: public Data {
  Q_OBJECT;
public:
  TOC(Data *parent=0);
  virtual ~TOC();
  QMap<int, TOCEntry> const &entries() const;
  TOCEntry const &entry(int pgno) const; // null Entry if none
  bool contains(int startPage) const;
  void addEntry(int startPage, TOCEntry const &entry);
  int nextPage() const;
  bool ok() const; // true if last save ok
  void setTitle(int startPage, QString title);
  void setSheetCount(int startPage, int sheetCount);
protected:
  virtual void loadMore(QVariantMap const &src);
  virtual void saveMore(QVariantMap &dst) const;
private:
  QMap<int, TOCEntry> entries_;
};

typedef DataFile<TOC> TOCFile;

#endif
