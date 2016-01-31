// File/ResLoader.cpp - This file is part of eln

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

// ResLoader.C

#include "ResLoader.h"
#include "Resource.h"
#include "Assert.h"
#include "WebGrab.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QVariant>
#include <QProgressDialog>
#include <QProcess>
#include <QTemporaryFile>
#include <QTextDocument>
#include <QTextStream>

QNetworkAccessManager &ResLoader::networkAccessManager() {
  static QNetworkAccessManager n;
  return n;
}

ResLoader::ResLoader(Resource *parent, bool convertHtmlToPdf):
  QObject(parent), parentRes(parent), convertHtmlToPdf(convertHtmlToPdf) {
  ASSERT(parentRes);
  
  ok = false;
  err = false;
  dst = 0;
  proc = 0;
  qnr = 0;
  redirectCount = 0;

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
  startDownload();
} 

void ResLoader::startDownload() {
  if (!dst->open(QFile::WriteOnly)) {
    qDebug() << "ResLoader: Cannot open dst";
    err = true;
    return;
  }

  QNetworkRequest rq(src);
  rq.setRawHeader("User-Agent", "Mozilla Firefox");

  qnr = networkAccessManager().get(rq);
  connect(qnr, SIGNAL(finished()), SLOT(qnrFinished()), Qt::QueuedConnection);
  connect(qnr, SIGNAL(readyRead()), SLOT(qnrDataAv()), Qt::QueuedConnection);
}

ResLoader::~ResLoader() {
  if (qnr)
    delete qnr; // really? doubtful
}

bool ResLoader::isComplete() const {
  return ok;
}

bool ResLoader::isFailed() const {
  return err;
}

QString ResLoader::mimeType() const {
  QString mime = qnr->header(QNetworkRequest::ContentTypeHeader).toString();
  int idx = mime.indexOf(";");
  return (idx>=0) ? mime.left(idx) : mime;
}

void ResLoader::qnrDataAv() {
  if (ok || err)
    return;
  QByteArray buf(65536, 0);
  while (true) {
    qint64 n = qnr->read(buf.data(), 65536);
    if (n>0) {
      dst->write(buf.data(), n);
      if (n<65536)
	break;
    } else if (n==0) {
      break;
    } else {
      qDebug() << "ResLoader: read<0 !?";
      dst->close();
      qnr->close();
      err = true;
      emit finished();
      break;
    }
  }
}

static bool hostMatch(QString a, QString b) {
  if (a==b)
    return true;
  if (a.startsWith("www.") && a.mid(4)==b)
    return true;
  if (b.startsWith("www.") && b.mid(4)==a)
    return true;
  return false;
}

void ResLoader::qnrFinished() {
  if (ok || err) // already finished
    return;

  dst->close();
  qnr->close(); // needed?

  if (qnr->error()) {
    qDebug() << "ResLoader " << src.toString() << ": qnr error" << qnr->error();
    err = true;
  }

  if (err) {
    emit finished();
    return;
  }
  
  QVariant attr = qnr->attribute(QNetworkRequest::RedirectionTargetAttribute);
  if (!attr.toString().isEmpty()) {
    if (++redirectCount >= 10) {
      qDebug() << "ResLoader: Too many redirects";
      err = true;
    } else {
      qnr->deleteLater();
      qnr = 0;
      QUrl newUrl(attr.toString());
      if (hostMatch(newUrl.host(), src.host()) && parentAlive()) {
	qDebug() << "Accepting redirect of " << src.toString()
		 << "to" << newUrl.toString();
	src = newUrl;
	parentRes->setSourceURL(src);
	startDownload();
	return;
      } else {
	qDebug() << "Cross-site redirect: refusing" << newUrl.toString();
	err = true;
      }
    }
  }

  if (err) {
    emit finished();
    return;
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
  qDebug() << "ResLoader: process error for " << src << proc->error();
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
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
	  this, SLOT(processFinished()));
  connect(proc, SIGNAL(error(QProcess::ProcessError)),
	  this, SLOT(processError()));
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
  args.append("-480");
  args.append(src.toString());
  args.append(parentRes->archivePath());
  if (!parentRes->previewFilename().isEmpty())
    args.append(parentRes->previewPath());
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
