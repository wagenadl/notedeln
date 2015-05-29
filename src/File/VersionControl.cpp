// File/VersionControl.cpp - This file is part of eln

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

// VersionControl.C

#include "App.h"
#include "VersionControl.h"
#include <QStringList>
#include <QMessageBox>
#include <QEventLoop>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QPushButton>

#ifdef Q_OS_LINUX
#include <sys/types.h>
#include <signal.h>
#endif

#define VC_TIMEOUT 300 // seconds

namespace VersionControl {
bool runVC(QString vccmd, QString subcmd, QStringList args, QString label,
             QString *stdo=0, QString *stde=0) {
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
  args.push_front(subcmd);
  process.start(vccmd, args);
  process.closeWriteChannel();
  if (!process.waitForStarted()) {
    if (stde)
      *stde += vccmd + ": could not start command: " + args.join(" ") + "\n";
    qDebug() << "runVC: could not start command" << vccmd << args.join(" ");
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
    if (stde)
      *stde += se;
    if (!se.isEmpty())
      qDebug() << "(" << vccmd << subcmd << ") " << se;

    QString so = process.readAllStandardOutput();
    allout += so;
    if (stdo)
      *stdo += so;
    if (!so.isEmpty())
      qDebug() << "(" << vccmd << subcmd << ") " << so;

    if (!se.isEmpty() || !so.isEmpty())
      box.setText(label + "\n" + allout);
  }

  QString se = process.readAllStandardError();
  if (stde)
    *stde += se;
  if (!se.isEmpty())
    qDebug() << "(" << vccmd << subcmd << ") " << se;
  
  QString so = process.readAllStandardOutput();
  if (stdo)
    *stdo += so;
  if (!so.isEmpty())
    qDebug() << "(" << vccmd << subcmd << ") " << so;

  if (stde)
    stde->replace(QRegExp("\\s*Traceback.*"), "");
  
  if (process.state()!=QProcess::NotRunning) {
    qDebug() << vccmd << subcmd << " failed\n";
    return false;
  }
  qDebug() << "process exit" << process.exitStatus() << process.exitCode();
  return process.exitStatus()==QProcess::NormalExit && process.exitCode()==0;
}


bool runBzr(QString cmd, QStringList args, QString label,
              QString *stdo=0, QString *stde=0) {
  return runVC("bzr", cmd, args, label, stdo, stde);
}

bool runGit(QString cmd, QStringList args, QString label,
              QString *stdo=0, QString *stde=0) {
  return runVC("git", cmd, args, label, stdo, stde);
}
  
bool update(QString path, QString program) {
  if (program == "")
    return false;

  QString cwd = QDir::currentPath();
  QString se;

  QDir::setCurrent(path);
  bool success = false;
  if (program == "bzr") 
    success = runBzr("update", QStringList(), "Updating with bzr...", 0, &se);
  else if (program == "git")
    success = runGit("pull", QStringList(), "Updating with git...", 0, &se);
  QDir::setCurrent(cwd);

  if (se.isEmpty())
    se = "(no message)";
  if (!success) {
    QMessageBox mb(QMessageBox::Warning, "eln version control",
		   "Update of '" + path + "' failed.", 0);
    QAbstractButton *editb
      = mb.addButton("Edit anyway", QMessageBox::DestructiveRole);
    QAbstractButton *rob
      = mb.addButton("Open read-only", QMessageBox::AcceptRole);
    QAbstractButton *quitb
      = mb.addButton("Quit", QMessageBox::RejectRole);
    mb.setDetailedText(se);
    mb.exec();
    QAbstractButton *r = mb.clickedButton();
    if (r==editb) {
      // edit anyway
    } else if (r==rob) {
      // open read only
      QMessageBox::warning(0, "eln missing feature",
                           "Read-only access is not yet implemented."
                           " Opening in the normal way."
                           " Please be careful not to change anything!");
    } else {
      // QApplication::quit();  // should we do this?
      exit(1); // immediate and total abort
    }
  }
  return success;
}
  
bool commit(QString path, QString program) {
  bool success = false;
  QString cwd = QDir::currentPath();
  QString se;
  QDir::setCurrent(path);
  if (program == "") {
    return false;
  } else if (program == "bzr") {
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
  } else if (program == "git") {
    /* The logic is:
       (1) we run git status -s
       (2) if that returns any text at all, we first do "add", then "commit",
           and finally "push".
       (3) errors at any stage cause us to give up immediately
     */
    QString out;
    success = runGit("status", QStringList() << "-s",
                     "Checking git status...", &out, &se);
    bool need = !out.isEmpty();
    if (need && success)
      success = runGit("add" , QStringList() << "-A",
                       "Adding files to git...", 0, &se);
    if (need && success)
      success = runGit("commit", QStringList() << "-meln elnautocommit",
                       "Committing locally using git...", 0, &se);
    if (need && success)
      success = runGit("push", QStringList(),
                       "Pushing changes using git...", 0, &se);
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
