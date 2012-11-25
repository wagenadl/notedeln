// Resource.C

#include "Resource.H"
#include "ResLoader.H"
#include <QImage>
#include <QDebug>

static Data::Creator<Resource> c("res");

Resource::Resource(Data *parent): Data(parent) {
  setType("res");
  refcnt = 0;
  loader = 0;
}

Resource::~Resource() {
}

QString Resource::tag() const {
  return tag_;
}

QUrl Resource::sourceURL() const {
  return src;
}

QString Resource::previewFilename() const {
  return prev;
}

QString Resource::archiveFilename() const {
  return arch;
}

QString Resource::title() const {
  return ttl;
}

QString Resource::description() const {
  return desc;
}

int Resource::refCount() const {
  return refcnt;
}

void Resource::setTag(QString s) {
  tag_ = s;
  markModified();
}

void Resource::setSourceURL(QUrl u) {
  src = u;
  markModified();
}

void Resource::setPreviewFilename(QString s) {
  prev = s;
  markModified();
  
}

void Resource::setArchiveFilename(QString s) {
  arch = s;
  markModified();
}

void Resource::setTitle(QString s) {
  ttl = s;
  markModified();
}

void Resource::setDescription(QString s) {
  desc = s;
  markModified();
}

void Resource::setRefCount(int c) {
  refcnt = c;
  markModified();
}

void Resource::incRefCount() {
  refcnt++;
  markModified();
}

bool Resource::decRefCount() {
  if (refcnt>0)
    refcnt--;
  markModified();
  return refcnt==0;
}

//////////////////////////////////////////////////////////////////////
void Resource::setRoot(QDir d) {
  dir = d;
}

bool Resource::hasArchive() const {
  return !arch.isEmpty() && dir.exists(arch) && !loader;
}

bool Resource::hasPreview() const {
  return !prev.isEmpty() && dir.exists(prev) && !loader;
}

QString Resource::archivePath() const {
  return dir.absoluteFilePath(arch);
}

QString Resource::previewPath() const {
  return dir.absoluteFilePath(prev);
}

//////////////////////////////////////////////////////////////////////
bool Resource::importImage(QImage img) {
  if (arch.isEmpty())
    arch = tag_ + ".png";
  ensureDir();
  bool ok = img.save(archivePath());
  markModified();
  return ok;
}

bool Resource::import() {
  if (arch.isEmpty())
    arch = tag_;
  ensureDir();
  ResLoader *l = new ResLoader(src, archivePath(), this);
  bool ok = l->getNowDialog();
  if (!ok)
    dir.remove(arch);
  delete l;
  return ok;
}

void Resource::getArchive() {
  if (loader)
    return; // can't start another one
  if (arch.isEmpty())
    arch = tag_;
  loader = new ResLoader(src, archivePath(), this);
  connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
}
  
void Resource::getArchiveAndPreview() {
  if (loader)
    return; // can't start another one
  if (arch.isEmpty())
    arch = tag_;
  if (prev.isEmpty())
    prev = tag_ + ".png";
  loader = new ResLoader(src, archivePath(), previewPath(), this);
  connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
}

void Resource::getPreviewOnly() {
  if (loader)
    return; // can't start another one
  if (prev.isEmpty())
    prev = tag_ + ".png";
  ensureDir();
  loader = new ResLoader(src, "", previewPath(), this);
  connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
}

void Resource::downloadFinished() {
  if (loader->failed()) {
    if (!arch.isEmpty())
      dir.remove(arch);
    if (!prev.isEmpty())
      dir.remove(prev);
  }
  loader->deleteLater();
  loader = 0;
  emit finished();
}

void Resource::ensureDir() {
  qDebug() << "Resource::ensureDir" << dir.absolutePath();
  if (!dir.exists())
    QDir::root().mkpath(dir.absolutePath());
}
