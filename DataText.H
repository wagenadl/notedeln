// DataText.H

#ifndef DATATEXT_H

#define DATATEXT_H

#include "Data.H"

class DataText: public Data {
public:
  DataText(QDomElement defn);
  DataText(QString tag, QDomElement parent, QString type="text");
  virtual ~DataText();
  QString text() const;
  void setText(QString);
protected:
  QDomText textnode;
};

#endif
