// ResLoader.H

#ifndef RESLOADER_H

#define RESLOADER_H

#include <QObject>
#include <QUrl>
#include <QFile>

class ResLoader: public QObject {
  Q_OBJECT;
public:
  /* These constructors should be replaced by constructors that just
     take a Resource *, and that have the option of changing the
     archive filename. */
  ResLoader(QUrl const &src, QString dst, QObject *parent=0);
  ResLoader(QUrl const &src, QString dst, QString prevDst, QObject *parent=0);
  virtual ~ResLoader();
  bool getNow(double timeout_s=-1); // true if finished without error
  bool getNowDialog(double delay_s = 0.2);
  bool complete() const; // true if finished without error
  bool failed() const; // true if finished with error
  // should we offer networkError() from qnr?
  // In case of error, the DST is deleted.
  QString mimeType() const; // may return "" if not (yet) known
signals:
  void finished(); // ok or not
  void progress(int); // percent, or -1 if unknown
private slots:
  void qnrFinished();
  void qnrDataAv();
  void qnrProgress(qint64, qint64);
  void processFinished();
  void processError();
private:
  void init();
  void startDownload();
  void makePdfAndThumb();
  bool makeThumb(QString mimetype); // return true if we're attempting it
  void startProcess(QString prog, QStringList args);
private:
  static class QNetworkAccessManager &nam();
  class QNetworkReply *qnr;
  class QProcess *proc;
  bool ok;
  bool err;
  QUrl src;
  QFile *dst;
  QFile *previewDst;
};

#endif
  
