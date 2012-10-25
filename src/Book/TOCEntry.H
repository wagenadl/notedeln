// TOCEntry.H

#ifndef TOCENTRY_H

#define TOCENTRY_H

#include "Data.H"

class TOCEntry: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString title READ title WRITE setTitle);
  Q_PROPERTY(int sheetCount READ sheetCount WRITE setSheetCount);
public:
  TOCEntry(Data *parent=0);
  virtual ~TOCEntry();
  // read properties
  QString title() const;
  int sheetCount() const;
  // write properties
  void setTitle(QString);
  void setSheetCount(int);
private:
  QString title_;
  int sheetCount_;
};

#endif
