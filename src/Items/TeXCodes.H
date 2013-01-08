// TeXCodes.H

#ifndef TEXCODES_H

#define TEXCODES_H

#include <QMap>
#include <QString>

class TeXCodes {
public:
  static bool contains(QString);
  static QString map(QString);
private:
  static QMap<QString, QString> const &chars();
};

#endif
