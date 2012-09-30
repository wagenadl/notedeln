// DataGfx.C

#include "DataGfx.H"

DataGfx::DataGfx(QDomElement defn): Data(defn) {
}

DataGfx::DataGfx(QString id, QDomElement parent, QString type):
  Data(id, parent, type) {
  defn.setAttribute("x", "0");
  defn.setAttribute("y", "0");
}


double DataGfx::x() const {
  return defn.attribute("x").toDouble();
}

double DataGfx::y() const {
  return defn.attribute("y").toDouble();
}

void DataGfx::setX(double x) {
  defn.setAttribute("x", QString::number(x));
  markModified();
}

void DataGfx::setY(double y) {
  defn.setAttribute("y", QString::number(y));
  markModified();
}

