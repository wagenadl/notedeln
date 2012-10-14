// ResourceManager.H

#ifndef RESOURCEMANAGER_H

#define RESOURCEMANAGER_H

#include <QUrl>
#include <QImage>
#include <QDir>
#include <QMap>

class ResourceManager: public QObject {
  Q_OBJECT;
public:
  ResourceManager(QString resdir, QObject *parent=0);
  QString import(QImage source, int pgno);
  QString import(QUrl source, int pgno); // this will wait until complete
  QString link(QUrl source, int pgno); // this will complete in b/g
  /* Return from import() and link() is a resource name.
     You must use exists() or complete() to check for success. */
  QString path(QString name) const; // maps a resource name to an abs. path
  bool exists(QString name) const; // true even while downloading
  bool complete(QString name) const; // true only after downloading complete
  bool outdated(QString name) const; // false for networked resources
private:
  QString newName(int pgno) const;
  QString newName(QUrl src, int pgno) const;
private slots:
  void downloadComplete(QString);
private:
  QDir dir;
  QMap<QString, QUrl> sources;
  QMap<QString, class ResourceLoader *> loaders;
  class QSignalMapper *mapper;
};

#endif
