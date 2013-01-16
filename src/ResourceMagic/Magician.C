// Magician.C

#include "Magician.H"

Magician::Magician() {
}

Magician::~Magician() {
}

bool Magician::matches(QString) const {
  return true;
}

QUrl Magician::webUrl(QString) const {
  return QUrl();
}

QUrl Magician::objectUrl(QString) const {
  return QUrl();
}

QString Magician::title(QString) const {
  return QString();
}

QString Magician::desc(QString) const {
  return QString();
}

SimpleMagician::SimpleMagician() {
  webUrlBuilder = "";
  objectUrlBuilder = "";
}

SimpleMagician::~SimpleMagician() {
}

QUrl SimpleMagician::webUrl(QString ref) const {
  if (webUrlBuilder.isEmpty())
    return QUrl();
  else
    return QUrl(webUrlBuilder.arg(ref));
}

QUrl SimpleMagician::objectUrl(QString ref) const {
  if (objectUrlBuilder.isEmpty())
    return QUrl();
  else
    return QUrl(objectUrlBuilder.arg(ref));
}

void SimpleMagician::setWebUrlBuilder(QString s) {
  webUrlBuilder = s;
}

void SimpleMagician::setObjectUrlBuilder(QString s) {
  objectUrlBuilder = s;
}

QUrl UrlMagician::objectUrl(QString s) const {
  if (s.startsWith("www"))
    return QUrl("http://" + s);
  else if (s.startsWith("/"))
    return QUrl("file://" + s);
  else
    return QUrl(s);
}
