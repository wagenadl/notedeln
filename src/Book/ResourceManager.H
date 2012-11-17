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
  QString import(QImage img, QUrl const *source=0);
  QString import(QUrl source); // this will wait until complete
  QString link(QUrl source); // this will complete in b/g
  /* Return from import() and link() is a resource name.
     You must use exists() or complete() to check for success. */
  QString resName(QUrl source) const; // returns resname for a given url, or ""
  QString path(QString resname) const; // maps a resource name to an abs. path
  QString thumbPath(QString resname) const; // if a resource has a thumbnail
  // associated with it, return its abs. path, else ""
  bool exists(QString resname) const; // true even while downloading
  bool complete(QString resname) const; // true only after downloading complete
  bool outdated(QString resname) const; // false for networked resources
private:
  QString newName() const;
  QString newName(QUrl src) const;
  void saveSources() const;
  void makePdfAndThumb(QString name);
private slots:
  void downloadComplete(QString);
  void processComplete(QString);
private:
  QDir dir;
  QMap<QString, QUrl> sources;
  QMap<QString, class ResourceLoader *> loaders;
  class QSignalMapper *mapper;
  QMap<QString, class QProcess *> processes;
  class QSignalMapper *procMapper;
};

#endif
