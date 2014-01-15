// Translate.H

#ifndef TRANSLATE_H

#define TRANSLATE_H

#include <QString>
#include <QMap>

class Translate {
public:
  static QString _(QString key);
  static QString language();
  static void setLanguage(QString l=""); // empty for default from OS
  // setLanguage must be called _before_ the first call to _().
private:
  static QMap<QString, QString> const &map();
  static QString &lang();
};

#endif
