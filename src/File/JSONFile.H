// JSONFile.H

#ifndef JSONFILE_H

#define JSONFILE_H

#include <QVariant>

namespace JSONFile {
  QVariantMap load(QString fn, bool *ok=0);
  QVariantMap read(QString json, bool *ok=0);
  bool save(QVariantMap const &src, QString fn);
  QString write(QVariantMap const &src);
};

#endif
