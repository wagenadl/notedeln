// FilePage.H

#ifndef FILEPAGE_H

#define FILEPAGE_H

#include "DataPage.H"

class FilePage {
public:
  FilePage(QString fn);
  virtual ~FilePage();
  static FilePage *create(QString fn);
  DataPage &data();
  bool ok() const;
  bool save() const; // false if failed to save
private:
  QDomDocument doc;
  DataPage *data_;
  QString fn;
};

#endif
