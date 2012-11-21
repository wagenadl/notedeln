// ResourceLoader.H

#ifndef RESOURCELOADER_H

#define RESOURCELOADER_H

#include <QUrl>
#include <QFile>
#include <QStringList>

class ResourceLoader: public QObject {
  Q_OBJECT;
public:
  ResourceLoader(QUrl const &src, QString dst, QObject *parent=0,
		 bool dontArchive=false);
  /* The "dontArchive" flag causes the loader to delete the destination
     file when the loader is destructed. This is useful if you only want
     to capture a thumbnail or if you only want to check existence of
     target. For local files, this prevents the file from being copied at all:
     instead, a temporary softlink is created (NYI).
  */
  virtual ~ResourceLoader();
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
  QFile dst;
  bool dontArchive;
};

#endif
