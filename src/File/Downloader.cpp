// File/Downloader.cpp - This file is part of NotedELN

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

// Downloader.cpp

#include "Downloader.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include "ElnAssert.h"

int Downloader::maxDownloadLength() {
  return 1000*1000*100;
}

QNetworkAccessManager &Downloader::networkAccessManager() {
  static QNetworkAccessManager *n = new QNetworkAccessManager();
  // This used to be simply "static QNetworkAcessManager n;", but at least
  // once did I see a crash when that QNAM was destroyed at program exit.
  // I don't understand why, but I think there is less harm in this
  // minor memory leak.
  return *n;
}

Downloader::Downloader(QUrl url, QObject *parent): QObject(parent), src(url) {
  ok = false;
  err = false;
  started = false;
  dst = 0;
  qnr = 0;
  redirectCount = 0;
}

void Downloader::start() {
  if (started) {
    qDebug() << "Downloader: already started";
  } else {
    startDownload();
  }
}

void Downloader::start(QString fn) {
  ASSERT(!qnr);
  if (started) {
    qDebug() << "Downloader: already started";
  } else {
    dst = new QFile(fn);
    qDebug() << "Downloader: " << fn;
    if (dst->open(QFile::WriteOnly)) {
      qDebug() << "Downloader " << fn << "starting" << src.toString();
      startDownload();
    } else {
      err = true;
      errs = "Could not open destination for writing";
      emit finished();
    }
  }
}

void Downloader::startDownload() {
  ASSERT(!qnr);
  
  dat.clear();
  N = 0;
  started = true;
  QNetworkRequest rq(src);
  rq.setRawHeader("User-Agent", "Mozilla Firefox");
  qnr = networkAccessManager().get(rq);
  connect(qnr, &QNetworkReply::finished,
          this, &Downloader::qnrFinished, Qt::QueuedConnection);
  connect(qnr, &QNetworkReply::readyRead, 
          this, &Downloader::qnrDataAv, Qt::QueuedConnection);
}

Downloader::~Downloader() {
  if (qnr)
    delete qnr; // really? doubtful
  if (dst) {
    if (!ok)
      dst->remove();
    delete dst;
  }
}

QUrl Downloader::source() const {
  return src;
}

bool Downloader::isComplete() const {
  return ok;
}

bool Downloader::isFailed() const {
  return err;
}

QString Downloader::mimeType() const {
  if (!qnr)
    return mimetype;
  QString mime = qnr->header(QNetworkRequest::ContentTypeHeader).toString();
  if (mime.isEmpty())
    return mimetype;
  int idx = mime.indexOf(";");
  return (idx>=0) ? mime.left(idx) : mime;
}

QByteArray Downloader::data() const {
  return dat;
}

void Downloader::qnrDataAv() {
  if (ok || err)
    return;
  ASSERT(qnr);
  mimetype = mimeType();
  QByteArray buf(65536, 0);
  while (true) {
    qint64 n = qnr->read(buf.data(), 65536);
    if (n>0) {
      N += n;
      if (N>maxDownloadLength()) {
        qDebug() << "Downloader: too long";
        if (dst)
          dst->close();
        qnr->close();
        err = true;
	errs = "Max download size exceeded";
	qnr->deleteLater();
	qnr = 0;
        emit finished();
        return;
      }
      if (dst) {
	dst->write(buf.data(), n);
      } else {
        dat += buf;
      }
      if (n<65536)
	break;
    } else if (n==0) {
      break;
    } else {
      qDebug() << "Downloader: read<0 !?";
      if (dst)
        dst->close();
      qnr->close();
      err = true;
      errs = QString("Network error: %1").arg(qnr->error());
      qnr->deleteLater();
      qnr = 0;
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

QString Downloader::error() const {
  return err ? errs : QString();
}

void Downloader::qnrFinished() {
  //  qDebug() << "Downloader finished for " << src.toString();
  //  system("ls");
  if (ok || err) // already finished
    return;

  ASSERT(qnr);
  
  qnr->close(); // needed?

  if (qnr->error()) {
    qDebug() << "Downloader " << src.toString() << ": qnr error" << qnr->error();
    err = true;
    errs = QString("Network error %1").arg(qnr->error());
  }

  if (err) {
    qnr->deleteLater();
    qnr = 0;
    emit finished();
    return;
  }
  
  QVariant attr = qnr->attribute(QNetworkRequest::RedirectionTargetAttribute);
  if (!attr.toString().isEmpty()) {
    if (++redirectCount >= 10) {
      qDebug() << "Downloader: Too many redirects";
      err = true;
      errs = "Too many redirects";
    } else {
      qnr->deleteLater();
      qnr = 0;
      QUrl newUrl(attr.toString());
      if (hostMatch(newUrl.host(), src.host())) {
	qDebug() << "Accepting redirect of " << src.toString()
		 << "to" << newUrl.toString();
	src = newUrl;
        startDownload();
	return;
      } else {
        qDebug() << "Cross-site redirect: refusing" << newUrl.host() << "from" << newUrl.toString() << "was" << src.host() << "from" << src.toString();
	err = true;
	errs = "Refusing cross-site redirect";
      }
    }
  }

  if (dst)
    dst->close();
  
  if (!err) 
    ok = true;

  qnr->deleteLater();
  qnr = 0;
  emit finished();
}
