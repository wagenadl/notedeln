// Resource.C

#include "Resource.H"
#include "ResLoader.H"
#include <QImage>
#include <QDebug>

static Data::Creator<Resource> c("res");

Resource::Resource(Data *parent): Data(parent) {
  setType("res");
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

static QString safeFileName(QString fn) {
  fn.replace(QRegExp("[^-\\w._]"), "-");
  if (fn.isEmpty())
    fn = "_empty_";
  return fn;
}

bool Resource::importImage(QImage img) {
  if (arch.isEmpty())
    arch = safeFileName(tag_ + ".png");
  ensureDir();
  bool ok = img.save(archivePath());
  markModified();
  return ok;
}

bool Resource::import() {
  if (arch.isEmpty())
    arch = safeFileName(tag_);
  ensureDir();
  ResLoader *l = new ResLoader(this);
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
    arch = safeFileName(tag_);
  ensureDir();
  loader = new ResLoader(this);
  connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
}
  
void Resource::getArchiveAndPreview() {
  if (loader)
    return; // can't start another one
  if (arch.isEmpty())
    arch = safeFileName(tag_);
  if (prev.isEmpty())
    prev = safeFileName(tag_ + ".png");
  ensureDir();
  loader = new ResLoader(this);
  connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
}

void Resource::getPreviewOnly() {
  if (loader)
    return; // can't start another one
  if (prev.isEmpty())
    prev = safeFileName(tag_ + ".png");
  if (!arch.isEmpty())
    setArchiveFilename("");
  ensureDir();
  loader = new ResLoader(this);
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
