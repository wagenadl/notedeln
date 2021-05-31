// Data/Resource.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Resource.C

#include "Resource.h"
#include "ResLoader.h"
#include <QImage>
#include <QDebug>
#include "ElnAssert.h"
#include "Notebook.h"
#include "TOC.h"
#include "TOCEntry.h"

static Data::Creator<Resource> c("res");

Resource::Resource(Data *parent): Data(parent) {
  setType("res");
  loader = 0;
  failed = false;
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
  if (tag_==s)
    return;
  tag_ = s;
  markModified();
}

void Resource::setSourceURL(QUrl u) {
  if (src==u)
    return;
  src = u;
  markModified();
}

void Resource::setPreviewFilename(QString s) {
  if (prev==s)
    return;
  prev = s;
  markModified();
  
}

void Resource::setArchiveFilename(QString s) {
  if (arch==s)
    return;
  arch = s;
  markModified();
}

void Resource::setTitle(QString s) {
  if (ttl==s)
    return;
  ttl = s;
  markModified();
}

void Resource::setDescription(QString s) {
  if (desc==s)
    return;
  desc = s;
  markModified();
}

//////////////////////////////////////////////////////////////////////
void Resource::setRoot(QDir d) {
  dir = d;
}

bool Resource::hasArchive() const {
  return !arch.isEmpty() && dir.exists(arch); // && !loader;
}

bool Resource::needsArchive() const {
  if (src.scheme()=="page")
    return false;
  else if (loader)
    return false;
  else
    return !hasArchive();
}  

bool Resource::hasPreview() const {
  return !prev.isEmpty() && dir.exists(prev) && !loader;
}

bool Resource::needsPreview() const {
  if (src.scheme()=="page")
    return false;
  else if (loader)
    return false;
  else
    return !hasPreview();
}
  

QString Resource::archivePath() const {
  return dir.absoluteFilePath(arch);
}

QString Resource::previewPath() const {
  return dir.absoluteFilePath(prev);
}

//////////////////////////////////////////////////////////////////////

static QString safeExtension(QString fn) {
  // returns extension including ".", or nothing. Removes nonword characters.
  QStringList bits = fn.split("/");
  if (bits.isEmpty())
    return "";
  fn = bits.last();
  int idx = fn.lastIndexOf(".");
  if (idx<0)
    return "";
  fn = fn.mid(idx+1);
  fn.replace(QRegExp("[^a-zA-Z0-9_]"), "");
  return "." + fn;
}
  
static QString safeBaseName(QString fn) {
  qDebug() << " safename" << fn;

  fn.replace(QRegExp("^http(s?)://"), "");
  fn.replace(QRegExp("^file://"), "");
  fn.replace(QRegExp("^//*"), "");
  fn.replace(QRegExp("//*$"), "");
  int idx = fn.lastIndexOf(".");
  int id0 = fn.lastIndexOf("/");
  if (idx>id0)
    fn = fn.left(idx); // drop extension

  QStringList bits = fn.split("/");
  if (bits.isEmpty())
    return "_";
  QString f_0 = "";
  QString f_n = "";
  if (bits.size()>=1) {
    f_0 = bits[0];
    if (f_0.size() > 32)
      f_0 = f_0.left(32);
  }
  if (bits.size()>=2) {
    f_n = bits.last();
    if (f_n.size() > 32)
      f_n = f_n.left(32);
  }

  fn = f_0 + "_" + f_n;

  fn.replace(QRegExp("[^a-zA-Z0-9_]"), "_");
  qDebug() << " -> " << fn;
  return fn;
}

void Resource::ensureArchiveFilename() {
  if (!arch.isEmpty())
    return;
  if (src.scheme()=="page")
    return;
  QString base = tag_;
  if (src.isLocalFile()) {
    // use extension from local file
    QStringList bits = src.path().split("/");
    QString leaf = bits.last();
    int idx = leaf.lastIndexOf(".");
    if (idx>=0) {
      int tagIdx = base.lastIndexOf(".");
      if (tagIdx>=0)
	base = base.left(tagIdx);
      base += leaf.mid(idx);
    }
  } else {
    qDebug() << "ensureArchiveFilename" << base <<src.path() << safeExtension(base);
    if (safeExtension(base).isEmpty())
      base += ".html";
  }
  setArchiveFilename(safeBaseName(base) + "-" + uuid() + safeExtension(base));
}

bool Resource::importImage(QImage img) {
  if (tag_.isEmpty())
    return false;
  if (arch.isEmpty()) {
    QString ext = safeExtension(src.path()).toLower();
    if (ext==".jpeg")
      ext = ".jpg";
    if (ext!=".jpg")
      ext = ".png";
    setArchiveFilename(safeBaseName(tag_) + "-" + uuid() + ext);
  }
  ensureDir();
  bool ok = false;
  if (src.isLocalFile()) {
    QString fn = src.path();
    ok = QFile::copy(fn, archivePath());
  } else {
    ok = img.save(archivePath());
  }
  markModified();
  return ok;
}

void Resource::setPreviewImage(QImage img) {
  ensureArchiveFilename();
  if (prev.isEmpty())
    setPreviewFilename(safeBaseName(tag_) + "-" + uuid() + "p.png");
  ensureDir();
  qDebug() << "setpreviewimage" << img.size();
  if (!img.isNull())
    img.save(previewPath());
  qDebug() << "image saved";
}
  

void Resource::getArchiveAndPreview() {
  qDebug() << "getarchiveandpreview for " << tag_ << loader
	   << needsArchive() << needsPreview() << src << src.isValid();
  if (loader)
    return; // can't start another one
  if (!needsArchive() && !needsPreview())
    return;
  ensureArchiveFilename();
  if (prev.isEmpty())
    setPreviewFilename(safeBaseName(tag_) + "-" + uuid() + "p.png");
  failed = false;
  if (!src.isValid())
    validateSource();
  qDebug() << "validated? " << src << src.isValid();
  if (src.isValid()) {
    loader = new ResLoader(this);
    connect(loader, SIGNAL(finished()), SLOT(downloadFinished()));
    loader->start();
  } else {
  }
}

static bool isHttpLike(QString s) {
  if (s.startsWith("http://")
      || s.startsWith("https://")
      || s.startsWith("file://")
      || s.startsWith("~/")
      || s.startsWith("/"))
    return true;

  QStringList spl = s.split("/");
  if (spl[0].startsWith("www.")
      || spl[0].endsWith(".com")
      || spl[0].endsWith(".net")
      || spl[0].endsWith(".org")
      || spl[0].endsWith(".edu"))
    return true;

  return false;
}

static QUrl urlFromTag(QString s) {
  if (s.startsWith("http://")
      || s.startsWith("https://")
      || s.startsWith("file://"))
    return QUrl(s);

  if (s.startsWith("/"))
    return QUrl::fromLocalFile(s);

  if (s.startsWith("~/")) {
    QString home = qgetenv("HOME");
    return QUrl::fromLocalFile(home + "/" + s.mid(2));
  }
  
  QStringList spl = s.split("/");  
  if (spl[0].startsWith("www.")
      || spl[0].endsWith(".com")
      || spl[0].endsWith(".net")
      || spl[0].endsWith(".org")
      || spl[0].endsWith(".edu"))
    return QUrl("http://" + s);

  return QUrl(s);
}

static bool isPubMed(QString s) {
  return QRegExp("\\d\\d\\d\\d\\d\\d*").exactMatch(s);
}

static bool isPageNumber(QString s) {
  return QRegExp("\\d\\d*[a-z]?").exactMatch(s);
}

static QUrl pageLink(QString s, Notebook *book) {
  ASSERT(book);
  TOC *toc = book->toc();
  ASSERT(toc);
  TOCEntry *te = toc->find(s);
  if (te) 
    return QUrl(QString("page://%1/%2/%3")
                .arg(s)
                .arg(te->uuid())
                .arg(te->sheetOf(s)));
  else
    return QUrl();
}

void Resource::validateSource() {
  // Right now, we only do http-like sources
  if (isHttpLike(tag())) 
    setSourceURL(urlFromTag(tag()));
  else if (isPubMed(tag()))
    setSourceURL("http://www.ncbi.nlm.nih.gov/pubmed/" + tag());
  else if (isPageNumber(tag()))
    setSourceURL(pageLink(tag(), book()));
  qDebug() << "validate" << tag() << isHttpLike(tag()) << src;
}

bool Resource::hasFailed() const {
  return failed;
}

bool Resource::inProgress() const {
  return loader;
}

void Resource::downloadFinished() {
  if (loader->isFailed()) {
    if (!arch.isEmpty())
      dir.remove(arch);
    if (!prev.isEmpty())
      dir.remove(prev);
    failed = true;
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


