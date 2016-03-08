// Catalog.h

#ifndef CATALOG_H

#define CATALOG_H

#include <QMap>
#include <QMultiMap>

class Catalog {
  // Catalog of files in the pages/ folder
public:
  Catalog(QString nbroot);
  bool isValid() const { return ok; }
  bool isClean() const;
  QMultiMap<int, QString> const &pageToFileMap() const { return pg2file; }
  QDateTime fileMod(QString) const;
  bool hasNotes(QString) const;
  QDateTime noteDirMod(QString) const;
  QStringList errors() const { return err; }
private:
  QMultiMap<int, QString> pg2file;
  QMap<QString, QDateTime> filemods;
  QMap<QString, QDateTime> notemods;
  bool ok;
  QStringList err;
};

#endif
