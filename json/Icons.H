// Icons.H

#ifndef ICONS_H

#define ICONS_H

#include <QMap>
#include <QIcon>

class Icons {
public:
  Icons();
  static Icons &icons();
  virtual ~Icons();
  bool contains(QString name);
  QIcon const &find(QString name);
private:
  static QString fn(QString name);
  static QString fn2(QString name);
  QMap<QString, QIcon> data;
  QIcon null;
};

#endif
