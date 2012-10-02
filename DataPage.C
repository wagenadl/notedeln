// DataPage.C

#include "DataPage.H"

DataPage::DataPage(QDomElement defn): Data(defn),
				      title_(defn.firstChildElement("title")) {
  for (QDomElement n=defn.firstChildElement();
       !n.isNull(); n=n.nextSiblingElement())
    blocks_.append(dynamic_cast<DataBlock*>(load(n)));
}

DataPage::DataPage(QString tag, QDomElement parent, QString type):
  Data(tag, parent, type),
  title_("title", defn) {
}

DataPage::~DataPage() {
  foreach (DataBlock *b, blocks_)
    delete b;
}

DataBlock *DataPage::addBlock(QString tag, QString type) {
  DataBlock *b = dynamic_cast<DataBlock*>(create(tag, defn, type));
  blocks_.append(b);
  return b;
}

QString DataPage::title() const {
  return title_.text();
}

void DataPage::setTitle(QString t) {
  title_.setText(t);
  markModified();
}
