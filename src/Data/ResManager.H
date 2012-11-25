// ResManager.H

#ifndef RESMANAGER_H

#define RESMANAGER_H

#include "Data.H"
#include "Resource.H"
#include <QUrl>
#include <QDir>
#include <QFile>

class ResManager: public Data {
  Q_OBJECT;
public:
  ResManager(Data *parent=0);
  virtual ~ResManager();
  Resource *byTag(QString) const; // 0 if not found
  Resource *byURL(QUrl) const; // 0 if not found
  void setRoot(QString);
  Resource *importImage(QImage img, QUrl source=QUrl());
  Resource *import(QUrl source);
  Resource *getArchiveAndPreview(QUrl source, QString altRes=""); // does not wait for completion
  Resource *getPreviewOnly(QUrl source, QString altRes=""); // does not wait for completion
  void dropResource(Resource *);
  Resource *newResource(QString altRes="");
private:
  QDir dir;
private:
  /* We could implement some maps and use the loadMore() mechanism to
     build them up, but that would require that addChild/deleteChild be
     made virtual, because otherwise there is no way to update the maps.
     So for now, we will not use any maps. Most pages will have so
     few resources that it isn't really worth it anyway.
  */
};

#endif
