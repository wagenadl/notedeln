// BackgroundVC.cpp

#include "BackgroundVC.H"
#include <QDebug>
#include <QProcess>
#include <QTimer>

#include "DataFile.H"
#include "DFBlocker.H"
#include "Assert.H"

#ifdef Q_OS_LINUX
#include <sys/types.h>
#include <signal.h>
#endif

BackgroundVC::BackgroundVC(QObject *parent): QObject(parent) {
  bzr = 0;
  guard = 0;
  maxt_s = 300;
  block = 0;
}
  
BackgroundVC::~BackgroundVC() {
  if (bzr) {
    qDebug() << "CAUTION! BackgroundVC deleted while still running!";
  }
}

void BackgroundVC::setTimeout(int s) {
  maxt_s = s;
}

bool BackgroundVC::commit(QString path1, QString program1) {
  if (program1 != "bzr") {
    qDebug() << "BackgroundVC can only do bzr";
    return false;
  }
  if (bzr) {
    qDebug() << "BackgroundVC can only do one task at once";
    return false;
  }
  path = path1;
  program = program1;

  if (!guard) {
    guard = new QTimer(this);
    connect(guard, SIGNAL(timeout()), SLOT(timeout()));
  }
  
  guard->setSingleShot(true);
  guard->setInterval(maxt_s*1000);
  guard->start();

  block = new DFBlocker(this);

  bzr = new QProcess(this);
  step = 0;
  bzr->setWorkingDirectory(path);
  connect(bzr, SIGNAL(finished(int, QProcess::ExitStatus)),
          SLOT(processFinished()));
  connect(bzr, SIGNAL(readyReadStandardError()),
          SLOT(processStderr()));
  connect(bzr, SIGNAL(readyReadStandardOutput()),
          SLOT(processStdout()));
  QStringList args; args << "add";
  bzr->start("bzr", args);
  bzr->closeWriteChannel();
  qDebug() << "BackgroundVC: started bzr" << args;
  return true;
}

void BackgroundVC::processStderr() {
  if (bzr)
    qDebug() << "BackgroundVC: (stderr) "
             << QString(bzr->readAllStandardError());
}

void BackgroundVC::processStdout() {
  if (bzr)
    qDebug() << "BackgroundVC: (stdout) "
             << QString(bzr->readAllStandardOutput());
}

void BackgroundVC::timeout() {
  if (!bzr)
    return;

  qDebug() << "BackgroundVC: timeout";

#ifdef Q_OS_LINUX
  ::kill(bzr->pid(), SIGINT);
  // Killing bzr with INT produces cleaner exit than with TERM...
#else
  bzr->kill();
  // ... but if we don't have POSIX, we have no choice.
#endif

  cleanup(false);
}

void BackgroundVC::cleanup(bool ok) {
  ASSERT(guard);
  ASSERT(block);
  ASSERT(bzr);
  guard->stop();
  block->deleteLater();
  block = 0;
  bzr->deleteLater();
  bzr = 0;
  qDebug() << "BackgroundVC: done " << ok;
  emit(done(ok));
}

void BackgroundVC::processFinished() {
  if (!bzr)
    return;

  if (bzr->exitCode()) {
    qDebug() << "BackgroundVC: process exited with code " << bzr->exitCode();
    cleanup(false);
  } else if (bzr->exitStatus()!=QProcess::NormalExit) {
    qDebug() << "BackgroundVC: process exited with abnormal status "
             << bzr->exitStatus();
    cleanup(false);
  }

  // so we're OK
  if (step==0) {
    // "add" step completed; let's commit
    step = 1;
    QStringList args; args << "commit";
    args << "-mautocommit";
    bzr->start("bzr", args);
    bzr->closeWriteChannel();
    qDebug() << "BackgroundVC: started bzr" << args;
  } else {
    // "commit" step completed. hurray!
    cleanup(true);
  }
}
