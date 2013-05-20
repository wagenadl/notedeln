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
  TOCEntry *find(int page) const; // returns entry containing the numbered page
                                  // ... or 0 if none does.
  bool contains(int startPage) const;
  TOCEntry *addEntry(class EntryData *data);
  bool deleteEntry(TOCEntry *);
  int newPageNumber() const;
  bool isLast(TOCEntry const *) const;
  void setBook(class Notebook *);
  virtual class Notebook *book() const; // pointer to parent book, if any.
protected:
  virtual void loadMore(QVariantMap const &src);
private: // hide these from general use
  void addChild(Data *);
  bool deleteChild(Data *);
  Data *takeChild(Data *);
  TOCEntry const *lastEntry() const;
private:
  typedef QMap<int, TOCEntry *> EntryMap;
  EntryMap entries_;
  Notebook *nb;
};

#endif
