// Magician.C

#include "Magician.H"
#include <QDebug>

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

bool Magician::objectUrlNeedsWebPage(QString) const {
  return false;
}

QUrl Magician::objectUrlFromWebPage(QString tag, QString) const {
  return objectUrl(tag);
}

//////////////////////////////////////////////////////////////////////

SimpleMagician::SimpleMagician() {
}

SimpleMagician::SimpleMagician(QVariantMap const &dict) {
  setMatcher(QRegExp(dict["re"].toString()));
  setWebUrlBuilder(dict["web"].toString());
  setObjectUrlBuilder(dict["object"].toString());
}

SimpleMagician::~SimpleMagician() {
}

bool SimpleMagician::matches(QString ref) const {
  bool ok = re.exactMatch(ref);
  qDebug() << "SimpleMagician" << re.pattern() << ref << ok;
  return ok;
}

void SimpleMagician::setMatcher(QRegExp r) {
  re = r;
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

bool UrlMagician::matches(QString s) const {
  return s.startsWith("http://")
    || s.startsWith("https://")
    || s.startsWith("file://")
    || s.startsWith("www.")
    || s.startsWith("/");
}

QUrl UrlMagician::objectUrl(QString s) const {
  if (s.startsWith("www."))
    return QUrl("http://" + s);
  else if (s.startsWith("/"))
    return QUrl("file://" + s);
  else
    return QUrl(s);
}

