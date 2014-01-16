// BackgroundVC.H

#ifndef BACKGROUNDVC_H

#define BACKGROUNDVC_H

#include <QObject>

class BackgroundVC: public QObject {
  Q_OBJECT;
public:
  BackgroundVC(QObject *parent);
  virtual ~BackgroundVC();
  bool commit(QString path, QString program); // returns true if started,
  // false if could not start
  void setTimeout(int secs);
  bool isBusy() const;
signals:
  void done(bool ok);
private slots:
  void processFinished();
  void processStderr();
  void processStdout();
  void timeout();
private:
  void cleanup(bool ok);
private:
  class QProcess *vc;
  class QTimer *guard;
  class DFBlocker *block;
  QString program;
  QString path;
  int step;
  int maxt_s;
};

#endif
