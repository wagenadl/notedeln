// File/ResLoader.cpp - This file is part of NotedELN

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

// ResLoader.C

#include "ResLoader.h"
#include "Resource.h"
#include "ElnAssert.h"
#include "WebGrab.h"

#include "Downloader.h"
#include <QTimer>
#include <QDebug>
#include <QVariant>
#include <QProcess>
#include <QTextDocument>
#include <QTextStream>

ResLoader::ResLoader(Resource *parent, bool convertHtmlToPdf):
  QObject(parent), parentRes(parent), convertHtmlToPdf(convertHtmlToPdf) {
  ASSERT(parentRes);
  
  ok = false;
  err = false;
  dst = 0;
  proc = 0;
  downloader = 0;

  src = parentRes->sourceURL();
  if (src.scheme()=="page")
    return;
  
  parentRes->ensureDir();
  dst = new QFile(parentRes->archivePath(), this);
}

void ResLoader::start() {
  if (src.scheme()=="page") {
    ok = true;
    emit finished();
    return;
  }
  if (parentRes->hasArchive())
    downloadFinished();
  else
    startDownload();
} 

void ResLoader::startDownload() {
  downloader = new Downloader(src, this);
  connect(downloader, &Downloader::finished,
          this, &ResLoader::downloadFinished);
  downloader->start(dst->fileName());
}

ResLoader::~ResLoader() {
  if (downloader)
    delete downloader;
}

bool ResLoader::isComplete() const {
  return ok;
}

bool ResLoader::isFailed() const {
  return err;
}

QString ResLoader::mimeType() const {
  return downloader ? downloader->mimeType() : "";
}

void ResLoader::downloadFinished() {
  if (ok || err) // already finished
    return;

  if (downloader && downloader->isFailed()) {
    qDebug() << "ResLoader " << src.toString()
             << ": downloader error" << downloader->error();
    err = true;
  }

  qDebug() << "ResLoader::downloadFinished" << err << src.toString()
	   << mimeType() << convertHtmlToPdf << downloader;

  if (err) {
    emit finished();
    return;
  }

  if (downloader && downloader->source() != src) {
    src = downloader->source();
    parentRes->setSourceURL(src);
  }
  
  if (mimeType()=="text/html") 
    getTitleFromHtml();

  if (mimeType()=="text/html" && convertHtmlToPdf) {
    if (makePdfAndPreview())
      return;
  } else {
    if (makePreview(mimeType()))
      return;
  }

  ok = true;
  emit finished();
}

void ResLoader::processError() {
  qDebug() << "ResLoader: process error for " << src << proc->error() << proc->program();
  qDebug() << proc->exitCode() << proc->exitStatus();
  qDebug() << proc->readAllStandardOutput();
  qDebug() << proc->readAllStandardError();
  err = true;
  qDebug() << "  failed";
  emit finished();
}  

void ResLoader::processFinished() {
  if (ok || err) // that means that processError took care of it already.
    return; 
  if (proc->exitStatus()!=QProcess::NormalExit || proc->exitCode()!=0) {
    // that didn't work
    processError();
  } else {
    ok = true;
    emit finished();
  }
}

void ResLoader::startProcess(QString prog, QStringList args) {
  proc = new QProcess(this);
  connect(proc, &QProcess::finished,
	  this, &ResLoader::processFinished);
  connect(proc, &QProcess::errorOccurred,
	  this, &ResLoader::processError);
  proc->start(prog, args);
  proc->closeWriteChannel();
  
}

bool ResLoader::parentAlive() const {
  return parent() == parentRes;
}

bool ResLoader::makePreview(QString mimetype) {
  if (!parentAlive())
    return false;
  if (parentRes->previewFilename().isEmpty())
    return false;

  if (mimetype.isEmpty()) {
    QStringList bits = src.path().split(".");
    if (!bits.isEmpty())
      mimetype = bits.last();
  }
  if (mimetype=="application/pdf" || mimetype=="application/x-pdf"
      || mimetype=="pdf") {
    QStringList args;
    QString prevFn = parentRes->previewFilename();
    if (!prevFn.endsWith(".png"))
      parentRes->setPreviewFilename(prevFn + ".png");
    QString prevPath = parentRes->previewPath();
    args.append("-l");
    args.append("1");
    args.append("-singlefile");
    args.append("-scale-to");
    args.append("480");
    args.append("-png");
    args.append(dst->fileName());
    args.append(prevPath.left(prevPath.size()-4));
    startProcess("pdftoppm", args);
    return true;
  } else {
    return false;
  }
}

bool ResLoader::makePdfAndPreview() {
  if (!parentAlive())
    return false;

  dst->remove(); // Remove the html file that we downloaded;
                 // we archive pdf instead.
  
  if (!parentRes->archiveFilename().endsWith(".pdf"))
    parentRes->setArchiveFilename(parentRes->archiveFilename() + ".pdf");

  QStringList args;
  args.append("-m");
  args.append(src.toString());
  args.append(parentRes->archivePath());
  if (!parentRes->previewFilename().isEmpty())
    args.append(parentRes->previewPath());
  qDebug() << "makepdfandpreview" << WebGrab::executable() << args;
  startProcess(WebGrab::executable(), args);
  return true;
}

QString ResLoader::mime2ext(QString mime) {
  if (mime=="application/pdf" || mime=="application/x-pdf")
    return "pdf";
  else if (mime=="text/html")
    return "html";
  else
    return "";
}

void ResLoader::getTitleFromHtml() {
  if (!parentAlive())
    return;
  if (!parentRes->title().isEmpty())
    return;

  if (!dst->open(QFile::ReadOnly))
    return;
  { QTextStream ts(dst);
    QString txt = ts.readAll();
    QTextDocument doc;
    doc.setHtml(txt);
    QString ttl = doc.metaInformation(QTextDocument::DocumentTitle);
    if (!ttl.isEmpty())
      parentRes->setTitle(ttl);
  }
  dst->close();
}
