// DataBlockText.C

#include "DataBlockText.H"

static DataMaker<DataBlockText> maker("textblock");

DataBlockText::DataBlockText(QDomElement defn):
  DataBlock(defn),
  text_(defn.firstChildElement("text")) {
}

DataBlockText::DataBlockText(QString tag, QDomElement parent, QString type):
  DataBlock(tag, parent, type),
  text_("text", defn) {
}

QString DataBlockText::text() const {
  return text_.text();
}

void DataBlockText::setText(QString t) {
  text_.setText(t);
  markModified();
}
