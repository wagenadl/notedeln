// DataChunk.C

#include "DataBlock.H"

static DataMaker<DataBlock> maker("block");

DataBlock::DataBlock(QDomElement defn): Data(defn) {
  for (QDomElement n=defn.firstChildElement("note");
       !n.isNull(); n=n.nextSiblingElement("note"))
    notes_.append(new DataNote(n));
}

DataBlock::DataBlock(QString id, QDomElement parent, QString type):
  Data(id, parent, type) {
}

DataBlock::~DataBlock() {
  foreach (DataNote *n, notes_)
    delete n;
}

QList<DataNote const *> DataBlock::notes() const {
  QList<DataNote const *> r;
  foreach (DataNote const *n, notes_)
    r.append(n);
  return r;
}

DataNote *DataBlock::addNote(QString id, double x, double y) {
  DataNote *n = new DataNote(id, defn);
  n->setX(x);
  n->setY(y);
  notes_.append(n);
  return n;
}

