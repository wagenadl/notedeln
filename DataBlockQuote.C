// DataBlockQuote.C

#include "DataBlockQuote.H"

static DataMaker<DataBlockQuote> maker("blockquote");

DataBlockQuote::DataBlockQuote(QString id, QDomElement parent, QString type):
  DataBlockText(id, parent, type),
  source_("source", defn) {
}

DataBlockQuote::DataBlockQuote(QDomElement defn):
  DataBlockText(defn),
  source_(defn.firstChildElement("source")) {
}

QString DataBlockQuote::source() const {
  return source_.text();
}

void DataBlockQuote::setSource(QString s) {
  source_.setText(s);
  markModified();
}
