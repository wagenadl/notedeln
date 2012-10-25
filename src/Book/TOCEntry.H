// TOCEntry.H

#ifndef TOCENTRY_H

#define TOCENTRY_H

#include "Data.H"

class TOCEntry: public Data {
  Q_OBJECT;
  Q_PROPERTY(int startPage READ startPage WRITE setStartPage);
  Q_PROPERTY(QString title READ title WRITE setTitle);
  Q_PROPERTY(int sheetCount READ sheetCount WRITE setSheetCount);
public:
  TOCEntry(Data *parent=0);
  virtual ~TOCEntry();
  // read properties
  int startPage() const;
  QString title() const;
  int sheetCount() const;
  // write properties
  void setStartPage(int);
  void setTitle(QString);
  void setSheetCount(int);
private:
  int startPage_;
  QString title_;
  int sheetCount_;
};

#endif
