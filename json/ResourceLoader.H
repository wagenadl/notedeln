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
  bool complete() const; // true if finished without error
  bool failed() const; // true if finished with error
  // should we offer networkError() from qnr?
  // In case of error, the DST is deleted.
signals:
  void finished(); // ok or not
private slots:
  void qnrFinished();
  void qnrDataAv();
private:
  static class QNetworkAccessManager &nam();
  class QNetworkReply *qnr;
  bool ok;
  bool err;
  QFile dst;
};

#endif
