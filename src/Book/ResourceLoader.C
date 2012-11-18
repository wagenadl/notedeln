// ResourceLoader.C

#include "ResourceLoader.H"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QVariant>
#include <QProgressDialog>
#include <QProcess>

QNetworkAccessManager &ResourceLoader::nam() {
  static QNetworkAccessManager n;
  return n;
}

ResourceLoader::ResourceLoader(QUrl const &src0, QString dst0, QObject *parent):
  QObject(parent), src(src0), dst(dst0) {
  QNetworkRequest rq(src);
  ok = err = false;
  proc = 0;
  qnr = 0;
  
  if (!dst.open(QFile::WriteOnly)) {
    qDebug() << "ResourceLoader: Cannot open dst";
    err = true;
    return;
  }
  
  qnr = nam().get(rq);
  connect(qnr, SIGNAL(finished()), SLOT(qnrFinished()), Qt::QueuedConnection);
  connect(qnr, SIGNAL(downloadProgress(qint64, qint64)),
	  SLOT(qnrProgress(qint64, qint64)), Qt::QueuedConnection);
  connect(qnr, SIGNAL(readyRead()), SLOT(qnrDataAv()), Qt::QueuedConnection);
}

bool ResourceLoader::complete() const {
  return ok;
}

bool ResourceLoader::failed() const {
  return err;
}

QString ResourceLoader::mimeType() const {
  return qnr->header(QNetworkRequest::ContentTypeHeader).toString();
}

void ResourceLoader::qnrProgress(qint64 n, qint64 m) {
  if (m<=0)
    emit progress(-1);
  else if (n<0)
    emit progress(-1);
  else
    emit progress(int(100*double(n)/double(m)));
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
      qDebug() << "ResourceLoader: Got redirect, can't deal";
      err = true;
    }
  }
  
  if (!err && mimeType().startsWith("text/html")) {
    makePdfAndThumb();
    return;
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

bool ResourceLoader::getNowDialog(double delay_s) {
  if (getNow(delay_s))
    return true;
  if (err)
    return false;

  QProgressDialog dlg("Downloading " + src.toString(),
		      "Cancel",
		      0, 100);
  connect(this, SIGNAL(progress(int)), &dlg, SLOT(setValue(int)));
  dlg.show();
  QEventLoop el(this);
  connect(qnr, SIGNAL(finished()), &el, SLOT(quit()));
  connect(&dlg, SIGNAL(canceled()), &el, SLOT(quit()));
  el.exec();
  if (qnr->isFinished() && !ok && !err)
    qnrFinished(); // since slots get called in arbitrary order, we must do this
  
  if (ok)
    return true;

  qnr->abort();
  qDebug() << "ResourceLoader: Aborted upon user request";
  err = true;
  dst.close();
  dst.remove();
  return false;
}
  
void ResourceLoader::processFinished() {
  qDebug() << "ResourceLoader: process finished for " << src;
  qDebug() << proc->readAllStandardOutput();
  qDebug() << proc->readAllStandardError();
  if (proc->exitStatus()!=QProcess::NormalExit
      || proc->exitCode()!=0) {
    // that didn't work
    ok = false;
    err = true;
    qDebug() << "  failed";
    QFile pdf(dst.fileName() + ".pdf");
    pdf.remove();
    QFile png(dst.fileName() + ".png");
    png.remove();
  } else {
    qDebug() << "  success";
    ok = true;
    err = false;
    // that did work
  }
  emit finished();
}

void ResourceLoader::makePdfAndThumb() {
  QStringList args;
  args.append(src.toString());
  args.append(dst.fileName()+".pdf");
  args.append(dst.fileName()+".png");
  proc = new QProcess(this);
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus )),
	  this, SLOT(processFinished()));
  qDebug() << "starting webgrab";
  // must deal with stdio from process?
  proc->start("webgrab", args);
  proc->closeWriteChannel();
}

  
