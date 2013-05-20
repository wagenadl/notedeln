// VersionControl.C

#include "App.H"
#include "VersionControl.H"
#include <QStringList>
#include <QMessageBox>
#include <QEventLoop>
#include <QProcess>
#include <QDebug>
#include <QDir>

#ifdef Q_OS_LINUX
#include <sys/types.h>
#include <signal.h>
#endif

#define VC_TIMEOUT 300 // seconds

namespace VersionControl {
  bool runBzr(char const *cmd, QStringList args, QString label,
              QString *stdout=0, QString *stderr=0) {
  QMessageBox box;
  // We're not using a progressdialog, because we have no clue
  // how long things will take.
  QString allout;
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
    if (stderr)
      *stderr += "bzr: could not start command: " + args.join(" ") + "\n";
    qDebug() << "runbzr: could not start command" << args.join(" ");
    return false;
  }
  for (int ntimeouts=0; ntimeouts<10*VC_TIMEOUT; ntimeouts++) {
    el.processEvents(); // this makes the messagebox show up
    if (process.waitForFinished(100)) {
      qDebug() << "process finished";
      break; // success or failure
    }
    if (box.isHidden()) {
      qDebug() << "killing process";
#ifdef Q_OS_LINUX
      ::kill(process.pid(), SIGINT);
      // Killing bzr with INT produces cleaner exit than with TERM...
#else
      process.kill();
      // ... but if we don't have POSIX, we have no choice.
#endif
      bool x = process.waitForFinished(500); // allow it some time to respond to signal
      qDebug() << "process finished" << x;
      break; // not good, but oh well
    }
    QString se = process.readAllStandardError();
    allout += se;
    if (stderr)
      *stderr += se;
    if (!se.isEmpty())
      qDebug() << "(bzr " << cmd << ") " << se;

    QString so = process.readAllStandardOutput();
    allout += so;
    if (stdout)
      *stdout += so;
    if (!so.isEmpty())
      qDebug() << "(bzr " << cmd << ") " << so;

    if (!se.isEmpty() || !so.isEmpty())
      box.setText(label + "\n" + allout);
  }

  QString se = process.readAllStandardError();
  if (stderr)
    *stderr += se;
  if (!se.isEmpty())
    qDebug() << "(bzr " << cmd << ") " << se;
  
  QString so = process.readAllStandardOutput();
  if (stdout)
    *stdout += so;
  if (!so.isEmpty())
    qDebug() << "(bzr " << cmd << ") " << so;

  if (stderr)
    stderr->replace(QRegExp("\\s*Traceback.*"), "");
  
  if (process.state()!=QProcess::NotRunning) {
    qDebug() << "bzr " << cmd << " failed\n";
    return false;
  }
  qDebug() << "process exit" << process.exitStatus() << process.exitCode();
  return process.exitStatus()==QProcess::NormalExit && process.exitCode()==0;
}

bool update(QString path, QString program) {
  bool success = false;
  QString cwd = QDir::currentPath();
  QString se;
  QDir::setCurrent(path);
  if (program == "bzr") 
    success = runBzr("update", QStringList(), "Updating with bzr...", 0, &se);
  QDir::setCurrent(cwd);
  if (se.isEmpty())
    se = "(no message)";
  if (!success) {
    QMessageBox mb(QMessageBox::Warning, "eln version control",
		   "Update of '" + path + "' failed.",
		   QMessageBox::Ignore);
    mb.addButton("Quit", QMessageBox::RejectRole);
    mb.setDetailedText(se);
    int r = mb.exec();
    qDebug() << r;
    if (r==0)
      exit(1); // immediate and total abort
  }
  return success;
}
  
bool commit(QString path, QString program) {
  bool success = false;
  QString cwd = QDir::currentPath();
  QString se;
  QDir::setCurrent(path);
  if (program == "bzr") {
    /* The logic is:
       (1) we run bzr status
       (2) if that returns any text at all, we first do "add", then "commit".
       (3) errors at any stage cause us to give up immediately
    */
    QString out;
    success = runBzr("status", QStringList(), "Checking bzr status...",
		     &out, &se);
    bool need = !out.isEmpty();
    if (need && success) 
      success = runBzr("add", QStringList(), "Adding files to bzr...", 0, &se);
    if (need && success) {
      QStringList args;
      args << "-meln autocommit";
      success = runBzr("commit", args, "Committing with bzr...", 0, &se);
    }
  }
  QDir::setCurrent(cwd);
  if (se.isEmpty())
    se = "(no message)";
  if (!success) {
    QMessageBox mb(QMessageBox::Warning, "eln version control",
		   "Commit of '" + path + "' failed.",
		   QMessageBox::Ignore);
    mb.setDetailedText(se);
    mb.exec();
  }
  return success;
}
};
