// DataPage.C

#include "DataPage.H"

DataPage::DataPage(QDomElement defn): Data(defn),
				      title_(defn.firstChildElement("title")) {
  for (QDomElement n=defn.firstChildElement();
       !n.isNull(); n=n.nextSiblingElement())
    blocks_.append(dynamic_cast<DataBlock*>(load(n)));
}

DataPage::DataPage(QString id, QDomElement parent, QString type):
  Data(id, parent, type),
  title_("title", defn) {
}

DataPage::~DataPage() {
  foreach (DataBlock *b, blocks_)
    delete b;
}

DataBlock *DataPage::addBlock(QString id, QString type) {
  DataBlock *b = dynamic_cast<DataBlock*>(create(id, defn, type));
  blocks_.append(b);
  return b;
}

QString DataPage::title() const {
  return title_.text();
}

void DataPage::setTitle(QString t) {
  title_.setText(t);
}
