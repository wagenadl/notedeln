// File/BackgroundVC.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// BackgroundVC.cpp

#include "BackgroundVC.h"
#include <QDebug>
#include <QProcess>
#include <QTimer>

#include "DataFile.h"
#include "DFBlocker.h"
#include "ElnAssert.h"
#include "VersionControl.h"

#ifdef Q_OS_LINUX
#include <sys/types.h>
#include <signal.h>
#endif

BackgroundVC::BackgroundVC(QObject *parent): QObject(parent) {
  vc = 0;
  guard = 0;
  maxt_s = 300;
  block = 0;
  step = -1;
}
  
BackgroundVC::~BackgroundVC() {
  if (vc) {
    qDebug() << "CAUTION! BackgroundVC deleted while still running!";
  }
}

void BackgroundVC::setTimeout(int s) {
  maxt_s = s;
}

bool BackgroundVC::commit(QString path1, QString program1) {
  if (VersionControl::isGloballyDisabled())
    return false;
  
  if (program1!="bzr" && program1!="git") {
    if (!program1.isEmpty())
      qDebug() << "BackgroundVC can only do bzr and git";
    return false;
  }
  if (vc) {
    qDebug() << "BackgroundVC can only do one task at once";
    return false;
  }
  path = path1;
  program = program1;

  if (!guard) {
    guard = new QTimer(this);
    connect(guard, &QTimer::timeout, this, &BackgroundVC::timeout);
  }
  
  guard->setSingleShot(true);
  guard->setInterval(maxt_s*1000);
  guard->start();

  block = new DFBlocker(this);

  vc = new QProcess(this);
  step = 0;
  vc->setWorkingDirectory(path);
  connect(vc, &QProcess::finished,
          this, &BackgroundVC::processFinished);
  connect(vc, &QProcess::readyReadStandardError,
          this, &BackgroundVC::processStderr);
  connect(vc, &QProcess::readyReadStandardOutput,
          this, &BackgroundVC::processStdout);
  if (program=="bzr") 
    vc->start("bzr", QStringList() << "add");
 else if (program=="git")
   vc->start("git", QStringList() << "add" << "-A");
 else
   qDebug() << "BackgroundVC: WHATVC!?!?" << program;
  vc->closeWriteChannel();
  return true;
}

void BackgroundVC::processStderr() {
  if (vc)
    qDebug() << "BackgroundVC: (stderr) "
             << QString(vc->readAllStandardError());
}

void BackgroundVC::processStdout() {
  if (vc)
    qDebug() << "BackgroundVC: (stdout) "
             << QString(vc->readAllStandardOutput());
}

void BackgroundVC::timeout() {
  if (!vc)
    return;

#ifdef Q_OS_LINUX
  ::kill(vc->processId(), SIGINT);
  // Killing vc with INT produces cleaner exit than with TERM...
#else
  vc->kill();
  // ... but if we don't have POSIX, we have no choice.
#endif

  cleanup(false);
}

void BackgroundVC::cleanup(bool ok) {
  ASSERT(guard);
  ASSERT(block);
  ASSERT(vc);
  guard->stop();
  block->deleteLater();
  block = 0;
  vc->deleteLater();
  vc = 0;
  emit(done(ok));
}

void BackgroundVC::processFinished() {
  if (!vc)
    return;

  if (vc->exitCode()) {
    qDebug() << "BackgroundVC: process exited with code " << vc->exitCode();
    cleanup(false);
    return;
  } else if (vc->exitStatus()!=QProcess::NormalExit) {
    qDebug() << "BackgroundVC: process exited with abnormal status "
             << vc->exitStatus();
    cleanup(false);
    return;
  }

  // so we're OK
  if (step==0) {
    // "add" step completed; let's commit (same for bzr and git)
    step = 1;
    vc->start(program, QStringList() << "commit" << "-mautocommit");
    vc->closeWriteChannel();
  } else if (step==1 && program=="git") {
    step = 2;
    vc->start(program, QStringList() << "push");
    vc->closeWriteChannel();
  } else {
    // final step completed. hurray!
    cleanup(true);
  }
}

bool BackgroundVC::isBusy() const {
  return vc!=0;
}
