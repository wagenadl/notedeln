// TeXCodes.C

#include "TeXCodes.H"
#include "Assert.H"
#include "JSONFile.H"

QMap<QString, QString> const &TeXCodes::chars() {
  static QMap<QString, QString> c;
  if (c.isEmpty()) {
    bool ok;
    QVariantMap v(JSONFile::load(":/TeXCodes.json", &ok));
    ASSERT(ok);
    foreach (QString k, v.keys())
      c[k] = v[k].toString();
  }
  return c;
}

bool TeXCodes::contains(QString k) {
  return chars().contains(k);
}

QString TeXCodes::map(QString k) {
  if (contains(k))
    return chars()[k];
  else
    return "";
}
