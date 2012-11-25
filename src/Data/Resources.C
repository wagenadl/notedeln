// Resources.C

#include "Resources.H"
#include <QImage>

static Data::Creator<Resources> c("resources");

Resources::Resources(Data *parent): Data(parent) {
  setType("resources");
}

Resources::~Resources() {
}

Resource *Resources::byTag(QString t) const {
  foreach (Resource *r, children<Resource>()) 
    if (r->tag()==t)
      return r;
  return 0;
}

Resource *Resources::byURL(QUrl t) const {
  foreach (Resource *r, children<Resource>()) 
    if (r->sourceURL()==t)
      return r;
  return 0;
}

void Resources::setRoot(QString d) {
  dir = d;
  foreach (Resource *r, children<Resource>())
    r->setRoot(d);
}

Resource *Resources::importImage(QImage img, QUrl source) {
  Resource *res = newResource();
  res->setSourceURL(source);
  res->importImage(img);
  return res;
}

Resource *Resources::import(QUrl source) {
  Resource *res = newResource();
  res->setSourceURL(source);
  res->import();
  return res;
}

Resource *Resources::getArchiveAndPreview(QUrl source, QString altRes) {
  Resource *res = newResource(altRes);
  res->setSourceURL(source);
  res->getArchiveAndPreview();
  return res;
}

Resource *Resources::getPreviewOnly(QUrl source, QString altRes) {
  Resource *res = newResource(altRes);
  res->setSourceURL(source);
  res->getPreviewOnly();
  return res;
}

void Resources::dropResource(Resource *r) {
  if (r->decRefCount()) {
    QFile a(r->archivePath()); a.remove();
    QFile p(r->previewPath()); p.remove();
    Q_ASSERT(deleteChild(r));
  }
}

Resource *Resources::newResource(QString altRes) {
  Resource *res = new Resource(this);
  res->setRoot(dir.absolutePath());
  res->incRefCount();
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
