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
  void linkCustom(QString resname, QUrl source, bool dontArchive=false);
  /* Return from import() and link() is a resource name.
     You must use contains() to check for success. */
  QString resName(QUrl source) const; // returns resname for a given url, or ""
  // if nonexistent
  QString path(QString resname) const; // maps a resource name to an abs. path
  // even if the resource doesn't exist
  QString previewPath(QString resname) const; // if a resource has a preview
  // (png) associated with it, return its abs. path, else ""
  QString pdfPath(QString resname) const; // if a resource has a pdf archive
  // associated with it, return its abs. path, else ""
  bool contains(QString resname) const; // true only after downloading complete
  // and successful
  bool mayContain(QString resname) const; // true even during downloading
  bool outdated(QString resname) const; // false for networked resources
  QUrl url(QString resname) const; // returns original url for resource
signals:
  void finished(QString resname); // emitted when download completes or fails
  // (Use contains() to check whether it was successful.)
private:
  QString newName() const;
  QString newName(QUrl src) const;
  void saveSources() const;
private slots:
  void downloadComplete(QString);
private:
  QDir dir;
  QMap<QString, QUrl> sources;
  QMap<QString, class ResourceLoader *> loaders;
  class QSignalMapper *mapper;
};

#endif
