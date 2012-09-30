// DataBlockquote.H

#ifndef DATABLOCKQUOTE_H

#define DATABLOCKQUOTE_H

#include "DataBlockText.H"

class DataBlockQuote: public DataBlockText {
public:
  DataBlockQuote(QDomElement defn);
  DataBlockQuote(QString id, QDomElement parent, QString type="blockquote");
  QString source() const;
  void setSource(QString s);
private:
  DataText source_;
};

#endif
