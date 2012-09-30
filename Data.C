// Data.C

#include "Data.H"

#define DATEFORMAT "yyyyMMdd.hhmmss"

static QDateTime str2date(QString str) {
  return QDateTime::fromString(str, DATEFORMAT);
}

static QString date2str(QDateTime dt) {
  return dt.toString(DATEFORMAT);
}

static QDateTime now() {
  return QDateTime::currentDateTime();
}

Data::Data(QString tag, QDomElement parent, QString type) {
  if (tag == "")
    tag = type;
  defn = parent.ownerDocument().createElement(tag);
  parent.appendChild(defn);
  if (type != tag)
    defn.setAttribute("type", type);
  defn.setAttribute("crea", date2str(now()));
  defn.setAttribute("mod", date2str(now()));
}

Data::Data(QDomElement defn): defn(defn) {
}

Data::~Data() {
}

Data *Data::load(QDomElement defn) {
  QString type = defn.hasAttribute("type")
    ? defn.attribute(type)
    : defn.tagName();
  Q_ASSERT_X(loaders().contains(type),
	     "Data::load",
	     "No loader for " + type);
  return loaders()[type](defn);
}

Data *Data::create(QString tag, QDomElement parent, QString type) {
  Q_ASSERT_X(typecreators().contains(type),
	     "Data::create",
	     "No creators for " + type);
  return creators()[type](tag, parent);
}

QString Data::type() const {
  return defn.hasAttribute("type")
    ? defn.attribute(type)
    : defn.tagName();
}

QString Data::tag() const {
  return defn.tagName();
}

QDateTime Data::created() const {
  return str2date(defn.attribute("crea"));
}
 
QDateTime Data::modified() const {
  return str2date(defn.attribute("mod"));
}
  
QMap<QString, Data *(*)(QDomElement)> &Data::loaders() {
  /* This trick with a static variable inside a static method guarantees
     that the map is initialized whenever it is referenced.
  */
  static QMap<QString, Data *(*)(QDomElement)> c;
  return c;
}
  
QMap<QString, Data *(*)(QString, QDomElement)> &Data::creators() {
  /* This trick with a static variable inside a static method guarantees
     that the map is initialized whenever it is referenced.
  */
  static QMap<QString, Data *(*)(QString, QDomElement)> c;
  return c;
}
  
void Data::registerLoader(QString typ, Data *(*foo)(QDomElement)) {
  loaders()[typ] = foo;
}

void Data::registerCreator(QString typ, Data *(*foo)(QString, QDomElement)) {
  creators()[typ] = foo;
}

void Data::markModified() {
  defn.setAttribute("mod", date2str(now()));
}
