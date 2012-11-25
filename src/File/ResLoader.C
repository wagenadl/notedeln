// ResLoader.C

#include "ResLoader.H"

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

QNetworkAccessManager &ResLoader::nam() {
  static QNetworkAccessManager n;
  return n;
}

void ResLoader::init() {
  ok = false;
  err = false;
  dst = 0;
  previewDst = 0;
  proc = 0;
  qnr = 0;
}

ResLoader::ResLoader(QUrl const &src0, QString dst0, QObject *parent):
  QObject(parent), src(src0) {
  init();
  dst = new QFile(dst0, this);
  startDownload();
}

ResLoader::ResLoader(QUrl const &src0, QString dst0, QString prevDst0,
		     QObject *parent):
  QObject(parent), src(src0) {
  init();
  if (dst0.isEmpty()) 
    dst = new QTemporaryFile(this);
  else
    dst = new QFile(dst0, this);
  previewDst = new QFile(prevDst0, this);
  startDownload();
}

void ResLoader::startDownload() {
  if (!dst->open(QFile::WriteOnly)) {
    qDebug() << "ResLoader: Cannot open dst";
    err = true;
    return;
  }

  QNetworkRequest rq(src);
  
  qnr = nam().get(rq);
  connect(qnr, SIGNAL(finished()), SLOT(qnrFinished()), Qt::QueuedConnection);
  connect(qnr, SIGNAL(downloadProgress(qint64, qint64)),
	  SLOT(qnrProgress(qint64, qint64)), Qt::QueuedConnection);
  connect(qnr, SIGNAL(readyRead()), SLOT(qnrDataAv()), Qt::QueuedConnection);
}

ResLoader::~ResLoader() {
}

bool ResLoader::complete() const {
  return ok;
}

bool ResLoader::failed() const {
  return err;
}

QString ResLoader::mimeType() const {
  return qnr->header(QNetworkRequest::ContentTypeHeader).toString();
}

void ResLoader::qnrProgress(qint64 n, qint64 m) {
  if (m<=0)
    emit progress(-1);
  else if (n<0)
    emit progress(-1);
  else
    emit progress(int(100*double(n)/double(m)));
}

void ResLoader::qnrDataAv() {
  if (ok || err)
    return;
  QByteArray buf(65536, 0);
  while (true) {
    qint64 n = qnr->read(buf.data(), 65536);
    if (n>0) {
      qDebug() << "ResLoader: read: " << n;
      dst->write(buf.data(), n);
      if (n<65536)
	break;
    } else if (n==0) {
      qDebug() << "ResLoader: read=0";
      break;
    } else {
      qDebug() << "ResLoader: read<0";
      qnr->close();
      err = true;
      emit finished();
      break;
    }
  }
}

void ResLoader::qnrFinished() {
  if (ok || err)
    return;

  dst->close();
  qnr->close(); // needed?

  if (qnr->error())
    err = true;

  if (!err) {
    QVariant attr = qnr->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!attr.toString().isEmpty()) {
      qDebug() << "ResLoader: Got redirect, can't deal";
      err = true;
    }
  }
  
  if (!err) {
    if (mimeType().startsWith("text/html")) {
      makePdfAndThumb();
      return;
    } else if (makeThumb(mimeType())) {
      return;
    }
  }

  if (!err)
    ok = true;

  emit finished();
}

bool ResLoader::getNow(double timeout_s) {
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

bool ResLoader::getNowDialog(double delay_s) {
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
  qDebug() << "ResLoader: Aborted upon user request";
  err = true;
  dst->close();
  return false;
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
  qDebug() << "ResLoader: process finished for " << src;
  qDebug() << proc->readAllStandardOutput();
  qDebug() << proc->readAllStandardError();
  if (ok || err) // that means that processError took care of it already.
    return; 
  if (proc->exitStatus()!=QProcess::NormalExit || proc->exitCode()!=0) {
    // that didn't work
    err = true;
    qDebug() << "  failed";
  } else {
    qDebug() << "  success";
    ok = true;
  }
  emit finished();
}

void ResLoader::startProcess(QString prog, QStringList args) {
  proc = new QProcess(this);
  qDebug() << "startProcess" << prog << args;
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
	  this, SLOT(processFinished()));
  connect(proc, SIGNAL(error(QProcess::ProcessError)),
	  this, SLOT(processError()));
  qDebug() << "starting" << prog;
  proc->start(prog, args);
  proc->closeWriteChannel();
}

bool ResLoader::makeThumb(QString mimetype) {
  Q_ASSERT(dst);
  Q_ASSERT(previewDst);
  qDebug() << "ResLoader::makeThumb" << mimetype;
  if (mimetype.isEmpty()) {
    QStringList bits = src.path().split(".");
    if (!bits.isEmpty())
      mimetype = bits.last();
  }
  if (mimetype.contains("application/pdf") ||
      mimetype.contains("application/x-pdf") || mimetype=="pdf") {
    QStringList args;
    args.append("-l");
    args.append("1");
    args.append("-singlefile");
    args.append("-scale-to");
    args.append("480");
    args.append("-png");
    args.append(dst->fileName());
    args.append(previewDst->fileName());
    startProcess("pdftoppm", args);
    return true;
  } else {
    return false;
  }
}

void ResLoader::makePdfAndThumb() {
  Q_ASSERT(dst);
  Q_ASSERT(previewDst);
  QStringList args;
  args.append("-480");
  args.append(src.toString());
  args.append(dst->fileName());
  args.append(previewDst->fileName());
  startProcess("webgrab", args);
}

  
