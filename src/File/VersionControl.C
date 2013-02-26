// VersionControl.C

#include "VersionControl.H"
#include <QStringList>
#include <QMessageBox>
#include <QEventLoop>
#include <QProcess>
#include <QDebug>
#include <QDir>

#include <sys/types.h>
#include <signal.h>

#define VC_TIMEOUT 300 // seconds

namespace VersionControl {
bool runBzr(char const *cmd, QStringList args, QString label) {
  QMessageBox box;
  // We're not using a progressdialog, because we have no clue
  // how long things will take.
  box.setWindowTitle("eln");
  box.setText(label);
  box.setWindowModality(Qt::ApplicationModal);
  box.setStandardButtons(QMessageBox::Cancel);
  box.show();
  QObject::connect(&box, SIGNAL(buttonClicked(QAbstractButton*)),
                   &box, SLOT(close()));
  QEventLoop el;
  QProcess process;
  args.push_front(cmd);
  process.start("bzr", args);
  process.closeWriteChannel();
  if (!process.waitForStarted()) {
    // error state
    qDebug() << "bzr " << cmd << " failed:\n"
             << process.readAllStandardError()
             << process.readAllStandardOutput();
    return false;
  }
  for (int ntimeouts=0; ntimeouts<10*VC_TIMEOUT; ntimeouts++) {
    el.processEvents(); // this makes the messagebox show up
    if (process.waitForFinished(100))
      break;
    if (box.isHidden()) {
      ::kill(process.pid(), SIGINT);
      process.waitForFinished(500); // allow it some time to respond to signal
      
      break;
    }
    QString stdout = process.readAllStandardOutput();
    QString stderr = process.readAllStandardError();
    if (!stdout.isEmpty())
      qDebug() << "(bzr " << cmd << ") " << stdout;
    if (!stderr.isEmpty())
      qDebug() << "(bzr " << cmd << ") " << stderr;
  }
  QString stdout = process.readAllStandardOutput();
  QString stderr = process.readAllStandardError();
  if (!stdout.isEmpty())
    qDebug() << "(bzr " << cmd << ") " << stdout;
  if (!stderr.isEmpty())
    qDebug() << "(bzr " << cmd << ") " << stderr;
  if (process.state()!=QProcess::NotRunning) {
    qDebug() << "bzr " << cmd << " failed\n";
    return false;
  }
  return process.exitStatus()==QProcess::NormalExit;
}

bool update(QString path, QString program) {
  bool success = false;
  QString cwd = QDir::currentPath();
  QDir::setCurrent(path);
  if (program == "bzr") 
    success = runBzr("update", QStringList(), "Updating with bzr...");
  QDir::setCurrent(cwd);
  return success;
}
  
bool commit(QString path, QString program) {
  bool success = false;
  QString cwd = QDir::currentPath();
  QDir::setCurrent(path);
  if (program == "bzr") {
    success = runBzr("add", QStringList(), "Adding files to bzr...");
    if (success) {
      QStringList args;
      args << "-meln autocommit";
      success = runBzr("commit", args, "Committing with bzr...");
    }
  }
  QDir::setCurrent(cwd);
  return success;
}
};
