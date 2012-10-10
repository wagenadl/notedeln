// DataNote.C

#include "DataNote.H"

static DataMaker<DataNote> maker("note");

DataNote::DataNote(QDomElement defn): DataText(defn) {
}

DataNote::DataNote(QString tag, QDomElement parent, QString type):
  DataText(tag, parent, type) {
  defn.setAttribute("x", "0");
  defn.setAttribute("y", "0");
}

double DataNote::x() const {
  return defn.attribute("x").toDouble();
}

double DataNote::y() const {
  return defn.attribute("y").toDouble();
}

void DataNote::setX(double x) {
  defn.setAttribute("x", QString::number(x));
  markModified();
}

void DataNote::setY(double y) {
  defn.setAttribute("y", QString::number(y));
  markModified();
}
