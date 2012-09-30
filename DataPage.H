// DataPage.H

#ifndef DATAPAGE_H

#define DATAPAGE_H

#include "Data.H"
#include <QList>
#include "DataBlock.H"
#include "DataText.H"

class DataPage: public Data {
public:
  DataPage(QDomElement defn);
  DataPage(QString id, QDomElement parent, QString type="page");
  virtual ~DataPage();
  DataBlock *addBlock(QString id, QString type);
  QString title() const;
  void setTitle(QString);
private:
  QList<DataBlock *> blocks_;
  DataText title_;
};

#endif
