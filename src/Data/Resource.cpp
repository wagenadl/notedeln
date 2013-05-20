// Data/Resource.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// Resource.C

#include "Resource.H"
#include "ResLoader.H"
#include <QImage>
#include <QDebug>
#include "ResourceMagic.H"
#include "Magician.H"

static Data::Creator<Resource> c("res");

Resource::Resource(Data *parent): Data(parent) {
  setType("res");
  loader = 0;
  magic = 0;
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

void Resource::ensureArchiveFilename() {
  if (!arch.isEmpty())
    return;
  QString base = tag_;
  if (src.isLocalFile()) {
    // use extension from local file
    QStringList bits = src.path().split("/");
    QString leaf = bits.last();
    int idx = leaf.lastIndexOf(".");
    if (idx>=0) {
      int tagIdx = tag_.lastIndexOf(".");
      if (tagIdx>=0)
	base = base.left(tagIdx);
      base += leaf.mid(idx);
    }
  }
  setArchiveFilename(safeFileName(base));
}

bool Resource::importImage(QImage img) {
  if (arch.isEmpty())
    setArchiveFilename(safeFileName(tag_ + ".png"));
  ensureDir();
  bool ok = img.save(archivePath());
  markModified();
  return ok;
}

bool Resource::import() {
  ensureArchiveFilename();
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
  ensureArchiveFilename();
  ensureDir();
  loader = new ResLoader(this);
  connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
}
  
void Resource::getArchiveAndPreview() {
  if (loader)
    return; // can't start another one
  ensureArchiveFilename();
  if (prev.isEmpty())
    setPreviewFilename(safeFileName(tag_ + ".png"));
  ensureDir();
  if (src.isValid()) {
    loader = new ResLoader(this);
    connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
  } else {
    if (magic)
      delete magic;
    magic = 0;
    doMagic();
  }    
}

void Resource::getPreviewOnly() {
  if (loader)
    return; // can't start another one
  if (prev.isEmpty())
    setPreviewFilename(safeFileName(tag_ + ".png"));
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
  markModified();
  emit finished();
}

void Resource::ensureDir() {
  if (!dir.exists())
    QDir::root().mkpath(dir.absolutePath());
}

void Resource::doMagic() {
  if (magic)
    magic->next();
  else
    magic = new ResourceMagic(tag_, this);

  while (!magic->isExhausted()) {
    src = QUrl();
    ttl = magic->title();
    desc = magic->desc();
    if (magic->webUrl().isValid()) {
      src = magic->webUrl();
      ensureArchiveFilename();
      ensureDir();
      loader = new ResLoader(this, false);
      connect(loader, SIGNAL(finished()), SLOT(magicWebUrlFinished()));
      return;
    } else if (magic->objectUrl().isValid()) {
      src = magic->objectUrl();
      ensureArchiveFilename();
      if (prev.isEmpty())
	setPreviewFilename(safeFileName(tag_ + ".png"));
      ensureDir();
      loader = new ResLoader(this);
      connect(loader, SIGNAL(finished()), SLOT(magicObjectUrlFinished()));
      return;
    //} else if (magic->keepAlways()) {
    //  markModified();
    //  emit finished();
    } else {
      ttl = "";
      desc = "";
      magic->next();
    }
  }
  if (!arch.isEmpty())
    dir.remove(arch);
  if (!prev.isEmpty())
    dir.remove(prev);
  src = QUrl();
  arch = "";
  prev = "";
  ttl = "";
  desc = "";
  markModified();
  emit finished(); // oh well
}

void Resource::magicWebUrlFinished() {
  if (loader->complete()) {
    // good work!
    loader->deleteLater();
    loader = 0;
    if (magic->objectUrlNeedsWebPage()) {
      QFile a(archivePath());
      if (a.open(QFile::ReadOnly)) {
	QString html = a.readAll();
	qDebug() << "magicurlfromwebpage" << html;
	src = magic->objectUrlFromWebPage(html);
	qDebug() << "  src = " << src.toString();
	a.close();
	loader = new ResLoader(this);
	connect(loader, SIGNAL(finished()), SLOT(magicObjectUrlFinished()));
      } else {
	markModified();
	emit finished();
      }
    } else if (magic->objectUrl().isValid()) {
      src = magic->objectUrl();
      ensureArchiveFilename();
      loader = new ResLoader(this);
      connect(loader, SIGNAL(finished()), SLOT(magicObjectUrlFinished()));
    } else {
      markModified();
      emit finished();
    }
  } else {
    doMagic(); // try next magician
  }
}

void Resource::magicObjectUrlFinished() {
  if (loader->complete() /* || magic->keepAlways()*/) {
    // good work!
    loader->deleteLater();
    loader = 0;
    if (magic->webUrl().isValid()) 
      src = magic->webUrl(); // restore web url
    markModified();
    emit finished();
  } else {
    doMagic(); // try next magician
  }
}

