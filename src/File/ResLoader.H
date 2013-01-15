// ResLoader.H

#ifndef RESLOADER_H

#define RESLOADER_H

#include <QObject>
#include <QUrl>
#include <QFile>

class ResLoader: public QObject {
  Q_OBJECT;
public:
  ResLoader(class Resource *parent, bool convertHtmlToPdf=true);
  virtual ~ResLoader();
  bool getNow(double timeout_s=-1); // true if finished without error
  bool getNowDialog(double delay_s = 0.2);
  bool complete() const; // true if finished without error
  bool failed() const; // true if finished with error
  // should we offer networkError() from qnr?
  // In case of error, the DST is deleted.
  QString mimeType() const; // may return "" if not (yet) known
  static QString mime2ext(QString);
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
  void startDownload();
  bool makePdfAndPreview();
  bool makePreview(QString mimetype); // return true if we're attempting it
  void startProcess(QString prog, QStringList args);
  bool parentAlive() const;
  void getTitleFromHtml();
private:
  static class QNetworkAccessManager &networkAccessManager();
  class Resource *parentRes;
  class QNetworkReply *qnr;
  class QProcess *proc;
  bool ok;
  bool err;
  QUrl src;
  QFile *dst;
  int redirectCount;
  bool convertHtmlToPdf;
};

#endif
  
