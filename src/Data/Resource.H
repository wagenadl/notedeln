// Resource.H

#ifndef RESOURCE_H

#define RESOURCE_H

#include "Data.H"
#include <QUrl>
#include <QDir>
#include <QFile>

class Resource: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString tag READ tag WRITE setTag);
  Q_PROPERTY(QUrl source READ sourceURL WRITE setSourceURL);
  Q_PROPERTY(QString preview READ previewFilename WRITE setPreviewFilename);
  Q_PROPERTY(QString archive READ archiveFilename WRITE setArchiveFilename);
  Q_PROPERTY(QString title READ title WRITE setTitle);
  Q_PROPERTY(QString desc READ description WRITE setDescription);
public: // strict Data functions
  Resource(Data *parent=0);
  virtual ~Resource();
  QString tag() const;
  QUrl sourceURL() const;
  QString previewFilename() const;
  QString archiveFilename() const;
  QString title() const;
  QString description() const;
  void setTag(QString);
  void setSourceURL(QUrl);
  void setPreviewFilename(QString);
  void setArchiveFilename(QString);
  void setTitle(QString);
  void setDescription(QString);
public: // functions to do with using the resource
  void setRoot(QDir); // this must be called before anything else works
  bool hasArchive() const;
  bool hasPreview() const;
  QString archivePath() const;
  QString previewPath() const;
public: // functions to do with actually acquiring a resource
  bool importImage(QImage); // true if successful. requires tag
  bool import(); // true if successful. requires tag, sourceUrl
  void getArchive(); // completes in b/g. requires tag, sourceUrl
  void getArchiveAndPreview(); // completes in b/g. requires tag, sourceUrl
  void getPreviewOnly(); // completes in b/g. requires tag, sourceUrl
signals:
  void finished(); // emitted when one of the getXX functions finishes
private slots:
  void downloadFinished();
private:
  void ensureDir();
private:
  QString tag_;
  QUrl src;
  QString prev;
  QString arch;
  QString ttl;
  QString desc;
private:
  class ResLoader *loader;
  QDir dir;
};

#endif
