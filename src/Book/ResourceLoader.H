// ResourceLoader.H

#ifndef RESOURCELOADER_H

#define RESOURCELOADER_H

#include <QUrl>
#include <QFile>

class ResourceLoader: public QObject {
  Q_OBJECT;
public:
  ResourceLoader(QUrl const &src, QString dst, QObject *parent=0);
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
private:
  static class QNetworkAccessManager &nam();
  class QNetworkReply *qnr;
  bool ok;
  bool err;
  QUrl src;
  QFile dst;
};

#endif
