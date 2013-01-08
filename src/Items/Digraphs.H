// Digraphs.H

#ifndef DIGRAPHS_H

#define DIGRAPHS_H

#include <QMap>
#include <QString>

class Digraphs {
public:
  static bool anyEndingWith(QString);
  static bool contains(QString);
  static QString map(QString);
private:
  static QMap<QString, QString> const &allEndingWith(QString);
  static QMap<QString, QMap<QString, QString> > const &chars();
};

#endif
