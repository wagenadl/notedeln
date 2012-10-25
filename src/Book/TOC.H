// TOC.H

#ifndef TOC_H

#define TOC_H

#include <QObject>
#include <QVariant>
#include <QDate>
#include "Data.H"
#include "DataFile.H"
#include "TOCEntry.H"

class TOC: public Data {
  Q_OBJECT;
public:
  TOC(Data *parent=0);
  virtual ~TOC();
  QMap<int, TOCEntry *> const &entries() const;
  TOCEntry *entry(int startPage) const; // assertion if not found
  bool contains(int startPage) const;
  void addEntry(int startPage, TOCEntry *entry);
  TOCEntry *newEntry(int startPage);
  bool deleteEntry(int startPage);
  int newPageNumber() const;
protected:
  virtual void loadMore(QVariantMap const &src);
  virtual void saveMore(QVariantMap &dst) const;
private:
  QMap<int, TOCEntry *> entries_;
};

#endif
