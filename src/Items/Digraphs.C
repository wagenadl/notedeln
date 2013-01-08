// Digraphs.C

#include "Digraphs.H"

#include "Assert.H"
#include "JSONFile.H"

QMap<QString, QMap<QString, QString> > const &Digraphs::chars() {
  static QMap<QString, QMap<QString, QString> > c;
  if (c.isEmpty()) {
    bool ok;
    QVariantMap v(JSONFile::load(":/Digraphs.json", &ok));
    ASSERT(ok);
    foreach (QString k, v.keys())
      c[k.right(1)][k] = v[k].toString();
  }
  return c;
}

bool Digraphs::anyEndingWith(QString k) {
  return chars().contains(k);
}

QMap<QString, QString> const &Digraphs::allEndingWith(QString k) {
  ASSERT(anyEndingWith(k));
  return chars().find(k).value();
}

bool Digraphs::contains(QString k) {
  return anyEndingWith(k.right(1))
    ? allEndingWith(k.right(1)).contains(k)
    : false;
}

QString Digraphs::map(QString k) {
  if (contains(k))
    return allEndingWith(k.right(1))[k];
  else
    return "";
}
