// EProcess.h

#ifndef EPROCESS_H

#define EPROCESS_H

#include <QStringList>

class EProcess {
public:
  EProcess();
  ~EProcess();
  void setWorkingDirectory(QString);
  void setNoStartMessage(QString);
  void setCommandAndArgs(QString, QStringList);
  void setWindowCaption(QString);
  bool exec();
  QString stdOut() const { return so; }
  QString stdErr() const { return se; }
private:
  QString msgNoStart;
  QString cmd;
  QStringList args;
  QString winCap;
  QString so;
  QString se;
  QString wd;
};

#endif
