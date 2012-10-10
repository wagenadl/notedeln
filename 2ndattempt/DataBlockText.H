// DataBlockText.H

#ifndef DATABLOCKTEXT_H

#define DATABLOCKTEXT_H

#include "DataBlock.H"

class DataBlockText: public DataBlock {
public:
  DataBlockText(QDomElement defn);
  DataBlockText(QString tag, QDomElement parent, QString type="textblock");
  QString text() const;
  void setText(QString t);
private:
  DataText text_;
};

#endif
