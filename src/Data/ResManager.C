// ResManager.C

#include "ResManager.H"
#include <QImage>

static Data::Creator<ResManager> c("resources");

ResManager::ResManager(Data *parent): Data(parent) {
  setType("resources");
}

ResManager::~ResManager() {
}

Resource *ResManager::byTag(QString t) const {
  foreach (Resource *r, children<Resource>()) 
    if (r->tag()==t)
      return r;
  return 0;
}

Resource *ResManager::byURL(QUrl t) const {
  foreach (Resource *r, children<Resource>()) 
    if (r->sourceURL()==t)
      return r;
  return 0;
}

void ResManager::setRoot(QString d) {
  dir = d;
  foreach (Resource *r, children<Resource>())
    r->setRoot(d);
}

Resource *ResManager::importImage(QImage img, QUrl source) {
  Resource *res = newResource();
  res->setSourceURL(source);
  res->importImage(img);
  return res;
}

Resource *ResManager::import(QUrl source) {
  Resource *res = newResource();
  res->setSourceURL(source);
  res->import();
  return res;
}

Resource *ResManager::getArchiveAndPreview(QUrl source, QString altRes) {
  Resource *res = newResource(altRes);
  res->setSourceURL(source);
  res->getArchiveAndPreview();
  return res;
}

Resource *ResManager::getPreviewOnly(QUrl source, QString altRes) {
  Resource *res = newResource(altRes);
  res->setSourceURL(source);
  res->getPreviewOnly();
  return res;
}

void ResManager::dropResource(Resource *r) {
  QFile a(r->archivePath()); a.remove();
  QFile p(r->previewPath()); p.remove();
  Q_ASSERT(deleteChild(r));
}

Resource *ResManager::newResource(QString altRes) {
  Resource *res = new Resource(this);
  res->setRoot(dir.absolutePath());
  if (altRes.isEmpty()) {
    int n = 1;
    while (byTag(QString::number(n)))
      n++;
    res->setTag(QString::number(n));
  } else {
    res->setTag(altRes);
  }
  return res;
}
