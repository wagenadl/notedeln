// Downloader.cpp

#include "Downloader.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>

int Downloader::maxDownloadLength() {
  return 1000*1000*100;
}

QNetworkAccessManager &Downloader::networkAccessManager() {
  static QNetworkAccessManager n;
  return n;
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
  if (started) {
    qDebug() << "Downloader: already started";
  } else {
    dst = new QFile(fn);
    if (dst->open(QFile::WriteOnly)) {
      startDownload();
    } else {
      err = true;
      emit finished();
    }
  }
}

void Downloader::startDownload() {
  dat.clear();
  N = 0;
  started = true;
  QNetworkRequest rq(src);
  rq.setRawHeader("User-Agent", "Mozilla Firefox");
  qnr = networkAccessManager().get(rq);
  connect(qnr, SIGNAL(finished()), SLOT(qnrFinished()), Qt::QueuedConnection);
  connect(qnr, SIGNAL(readyRead()), SLOT(qnrDataAv()), Qt::QueuedConnection);
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
    return "";
  QString mime = qnr->header(QNetworkRequest::ContentTypeHeader).toString();
  int idx = mime.indexOf(";");
  return (idx>=0) ? mime.left(idx) : mime;
}

void Downloader::qnrDataAv() {
  if (ok || err)
    return;

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
        emit finished();
        return;
      }
      if (dst)
        dst->write(buf.data(), n);
      else
        dat += buf;
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
  if (qnr->error())
    return QString("%1").arg(qnr->error());
  else
    return "";
}

void Downloader::qnrFinished() {
  if (ok || err) // already finished
    return;

  if (dst)
    dst->close();

  qnr->close(); // needed?

  if (qnr->error()) {
    qDebug() << "Downloader " << src.toString() << ": qnr error" << qnr->error();
    err = true;
  }

  if (err) {
    emit finished();
    return;
  }
  
  QVariant attr = qnr->attribute(QNetworkRequest::RedirectionTargetAttribute);
  if (!attr.toString().isEmpty()) {
    if (++redirectCount >= 10) {
      qDebug() << "Downloader: Too many redirects";
      err = true;
    } else {
      qnr->deleteLater();
      qnr = 0;
      QUrl newUrl(attr.toString());
      if (hostMatch(newUrl.host(), src.host()) && parent()) {
	qDebug() << "Accepting redirect of " << src.toString()
		 << "to" << newUrl.toString();
	src = newUrl;
        startDownload();
	return;
      } else {
	qDebug() << "Cross-site redirect: refusing" << newUrl.toString();
	err = true;
      }
    }
  }

  if (!err) 
    ok = true;

  emit finished();
}
