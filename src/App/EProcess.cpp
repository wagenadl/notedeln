// App/EProcess.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// EProcess.cpp

#include "EProcess.h"

#include <QMessageBox>
#include <QEventLoop>
#include <QProcess>
#include <QDebug>

#ifdef Q_OS_LINUX
#include <sys/types.h>
#include <signal.h>
#endif

EProcess::EProcess() {
}

EProcess::~EProcess() {
}

void EProcess::setWorkingDirectory(QString s) {
  wd = s;
}

void EProcess::setNoStartMessage(QString s) {
  msgNoStart = s;
}

void EProcess::setCommandAndArgs(QString c, QStringList a) {
  cmd = c;
  args = a;
}

void EProcess::setWindowCaption(QString c) {
  winCap = c;
}

bool EProcess::exec() {
  se = so = "";
  
  QMessageBox box;
  // We're not using a progressdialog, because we have no clue
  // how long things will take.
  QString allout;
  box.setWindowTitle("eln");
  box.setText(winCap);
  box.setWindowModality(Qt::ApplicationModal);
  box.setStandardButtons(QMessageBox::Cancel);
  box.show();
  QObject::connect(&box, SIGNAL(buttonClicked(QAbstractButton*)),
                   &box, SLOT(close()));
  QEventLoop el;
  QProcess process;
  if (!wd.isEmpty())
    process.setWorkingDirectory(wd);
  process.start(cmd, args);
  process.closeWriteChannel();
  if (!process.waitForStarted()) {
    QString msg = msgNoStart.isEmpty()
      ? "Could not start command: " + cmd
      : msgNoStart;
    se += msg + "\n";
    return false;
  }

  for (int ntimeouts=0; ntimeouts<10*300; ntimeouts++) {
    el.processEvents(); // this makes the messagebox show up
    if (process.waitForFinished(100)) {
      break; // success or failure
    }
    if (box.isHidden()) {
#ifdef Q_OS_LINUX
      ::kill(process.pid(), SIGINT);
      // Killing bzr with INT produces cleaner exit than with TERM...
#else
      process.kill();
      // ... but if we don't have POSIX, we have no choice.
#endif
      process.waitForFinished(500); // allow it some time to respond to signal
      break; // not good, but oh well
    }
    QString ste = process.readAllStandardError();
    allout += ste;
    se += ste;

    QString sto = process.readAllStandardOutput();
    allout += sto;
    so += sto;

    if (!ste.isEmpty() || !sto.isEmpty())
      box.setText(winCap + "\n" + allout);
  }

  QString ste = process.readAllStandardError();
  se += ste;
  
  QString sto = process.readAllStandardOutput();
  so += sto;

  se.replace(QRegExp("\\s*Traceback.*"), "");
  
  if (process.state()!=QProcess::NotRunning) {
    return false;
  }
  return process.exitStatus()==QProcess::NormalExit && process.exitCode()==0;
}
