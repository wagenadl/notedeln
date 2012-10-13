// ResourceLoader.C

#include "ResourceLoader.H"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QVariant>

QNetworkAccessManager &ResourceLoader::nam() {
  static QNetworkAccessManager n;
  return n;
}

ResourceLoader::ResourceLoader(QUrl const &src, QString dst0, QObject *parent):
  QObject(parent), dst(dst0) {
  QNetworkRequest rq(src);
  ok = err = false;
  if (!dst.open(QFile::WriteOnly)) {
    qDebug() << "ResourceLoader: Cannot open dst";
    err = true;
    qnr = 0;
    return;
  }
  
  qnr = nam().get(rq);
  connect(qnr, SIGNAL(finished()), SLOT(qnrFinished()));
  connect(qnr, SIGNAL(readyRead()), SLOT(qnrDataAv()));
  //if (qnr->isFinished())
    // in case the signal got emitted before our connection got made
    //qnrFinished(); 
}

bool ResourceLoader::complete() const {
  return ok;
}

bool ResourceLoader::failed() const {
  return err;
}

void ResourceLoader::qnrDataAv() {
  if (ok || err)
    return;
  QByteArray buf(65536, 0);
  while (true) {
    qint64 n = qnr->read(buf.data(), 65536);
    if (n>0) {
      qDebug() << "ResourceLoader: read: " << n;
      dst.write(buf.data(), n);
      if (n<65536)
	break;
    } else if (n==0) {
      qDebug() << "ResourceLoader: read=0";
      break;
    } else {
      qDebug() << "ResourceLoader: read<0";
      err = true;
      break;
    }
  }
}

void ResourceLoader::qnrFinished() {
  if (ok || err)
    return;

  dst.close();
  qnr->close(); // needed?

  if (qnr->error())
    err = true;

  if (!err) {
    QVariant attr = qnr->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!attr.toString().isEmpty()) {
      qDebug() << "ResourceLoader: Got redirect";
      err = true;
    }
  }
  
  if (err)
    dst.remove();
  else
    ok = true;

  emit finished();
}

bool ResourceLoader::getNow(double timeout_s) {
  if (ok)
    return true;
  if (err)
    return false;

  QEventLoop el(this);
  QTimer t(this);

  connect(&t, SIGNAL(timeout()), &el, SLOT(quit()));
  connect(qnr, SIGNAL(finished()), &el, SLOT(quit()));

  t.setSingleShot(true);
  if (timeout_s>=0)
    t.start(int(1e3*timeout_s));

  el.exec();

  if (qnr->isFinished() && !ok && !err)
    qnrFinished(); // since slots get called in arbitrary order, we must do this
  
  if (ok)
    return true;
  else
    return false; // this does *not* necessarily mean error
}
