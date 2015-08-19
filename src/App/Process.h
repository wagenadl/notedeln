// Process.h

#ifndef PROCESS_H

#define PROCESS_H

#include <QStringList>

class Process {
public:
  Process();
  ~Process();
  void setWorkingDirectory(QString);
  void setNoStartMessage(QString);
  void setCommandAndArgs(QString, QStringList);
  void setWindowCaption(QString);
  bool exec();
  QString stdout() const { return so; }
  QString stderr() const { return se; }
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
