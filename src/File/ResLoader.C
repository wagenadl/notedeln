// ResLoader.C

#include "ResLoader.H"
#include "Resource.H"
#include "Assert.H"

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
  dst = new QFile(parentRes->archivePath(), this);

  startDownload();
}

void ResLoader::startDownload() {
  if (!dst->open(QFile::WriteOnly)) {
    qDebug() << "ResLoader: Cannot open dst";
    err = true;
    return;
  }

  if (src.scheme() == "page") {
    dst->close();
    ok = true;
    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), SIGNAL(finished()));
    t->setSingleShot(true);
    t->start(1);
    return;
    // This absurd code ensures that Resource (and others?) have a chance to
    // connect to our finished() signal before we emit it.
  }

  QNetworkRequest rq(src);
  
  qnr = networkAccessManager().get(rq);
  connect(qnr, SIGNAL(finished()), SLOT(qnrFinished()), Qt::QueuedConnection);
  connect(qnr, SIGNAL(downloadProgress(qint64, qint64)),
	  SLOT(qnrProgress(qint64, qint64)), Qt::QueuedConnection);
  connect(qnr, SIGNAL(readyRead()), SLOT(qnrDataAv()), Qt::QueuedConnection);
}

ResLoader::~ResLoader() {
  if (qnr)
    delete qnr; // really? doubtful
}

bool ResLoader::complete() const {
  return ok;
}

bool ResLoader::failed() const {
  return err;
}

QString ResLoader::mimeType() const {
  QString mime = qnr->header(QNetworkRequest::ContentTypeHeader).toString();
  int idx = mime.indexOf(";");
  return (idx>=0) ? mime.left(idx) : mime;
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
      qDebug() << "ResLoader: read<0 !?";
      dst->close();
      qnr->close();
      err = true;
      emit finished();
      break;
    }
  }
}

void ResLoader::qnrFinished() {
  if (ok || err) // already finished
    return;

  dst->close();
  qnr->close(); // needed?

  if (qnr->error())
    err = true;

  if (err) {
    emit finished();
    return;
  }
  
  QVariant attr = qnr->attribute(QNetworkRequest::RedirectionTargetAttribute);
  if (!attr.toString().isEmpty()) {
    qDebug() << "ResLoader: Got redirect";
    if (++redirectCount >= 10) {
      qDebug() << "Too many redirects";
      err = true;
    } else {
      qnr->deleteLater();
      qnr = 0;
      QUrl newUrl(attr.toString());
      if (newUrl.host() == src.host() && parentAlive()) {
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

bool ResLoader::parentAlive() const {
  return parent() == parentRes;
}

bool ResLoader::makePreview(QString mimetype) {
  if (!parentAlive())
    return false;
  if (parentRes->previewFilename().isEmpty())
    return false;

  qDebug() << "ResLoader::makePreview" << mimetype;
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
  startProcess("webgrab", args);
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
