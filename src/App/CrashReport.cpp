// App/CrashReport.cpp - This file is part of eln

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

// CrashReport.cpp
#include <QDebug>
#include "CrashReport.h"

#if defined(Q_OS_UNIX) || defined(Q_OS_MAC)

#include <unistd.h>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Calltrace.h"
#include "Translate.h"

static int eln_pipe[2] = { -1, -1 };

#define CR_MARKER "---"

static void eln_ungrabsignals() {
  struct sigaction newHdlr;
  newHdlr.sa_handler = SIG_DFL;
  sigemptyset(&newHdlr.sa_mask);
  newHdlr.sa_flags = SA_RESETHAND;
  sigaction(SIGILL, &newHdlr, 0);
  sigaction(SIGABRT, &newHdlr, 0);
  sigaction(SIGFPE, &newHdlr, 0);
  sigaction(SIGSEGV, &newHdlr, 0);
  sigaction(SIGBUS, &newHdlr, 0);
  sigaction(SIGPIPE, &newHdlr, 0);
}

static void eln_sighandler(int sig) {
  eln_ungrabsignals();
  int fd = eln_pipe[1];
  if (fd<0)
    fd = 2; // i.e., stderr

  int n = 0;
  char buf[100];
  sprintf(buf, "Signal %i (%s) caught.", sig, strsignal(sig));
  n = write(fd, buf, strlen(buf));
  
  sprintf(buf, CR_MARKER "Backtrace: ");
  n = write(fd, buf, strlen(buf));
  n = write(1, buf, strlen(buf));
  QString trace = Calltrace::quick();
  char const *b = trace.toLatin1().constData();
  n = write(fd, b, strlen(b));
  n = write(1, b, strlen(b));
  if (n) { }

  if (fd!=2)
    close(fd);
  exit(128+sig);
}

static void eln_grabsignals() {
  struct sigaction newHdlr;
  newHdlr.sa_handler = &eln_sighandler;
  sigemptyset(&newHdlr.sa_mask);
  newHdlr.sa_flags = SA_RESETHAND;
  sigaction(SIGILL, &newHdlr, 0);
  sigaction(SIGABRT, &newHdlr, 0);
  sigaction(SIGFPE, &newHdlr, 0);
  sigaction(SIGSEGV, &newHdlr, 0);
  sigaction(SIGBUS, &newHdlr, 0);
  sigaction(SIGPIPE, &newHdlr, 0);
}

static void eln_crashreporter(int fd, QString email) {
  QString in;
  char buf[1024];
  int n;
  while (true) {
    n = read(fd, buf, 1023);
    if (n<=0)
      break;
    buf[n] = 0;
    n = write(2, buf, n);
    in += buf;
  }
  int status = -1;
  pid_t pid = wait(&status);
  if (pid<=0)
    status = -2;
  else if (WIFEXITED(status))
    status = WEXITSTATUS(status);
  else if (WIFSIGNALED(status))
    status = 128 + WTERMSIG(status);

  if (status<0) {
    qDebug() << "Crash status" << pid << status;
    exit(1);
  }

  if (status<128) {
    // ELN already reported
    exit(status);
  }

  if (status==128 + SIGTERM)
    exit(1);  // we don't report for SIGTERM

  QStringList bits = in.split(CR_MARKER);
  
  int argc = 1;
  char arg0[] = "eln_crash";
  char *argv[] = { arg0, 0 };
  QApplication app(argc, argv);

  QString s = bits.takeFirst().trimmed();
  QString hdr = s.isEmpty() ? "ELN was terminated in an usual way:"
    : "ELN suffered a fatal internal error and had to close:";
  
  QMessageBox mb(QMessageBox::Critical, "ELN", hdr,
                 QMessageBox::Close);

  QString msg;
  if (s.isEmpty()) {
    if (status>=128)
      msg = QString("    Terminated because of signal %1 (%2).")
        .arg(status-128).arg(strsignal(status-128));
    else
      msg = QString("    Terminated with exit code %1.").arg(status);
    msg += "\n\nIf you did not cause that to happen on purpose,"
      " please send a bug report to the author at " + email + "."
      "\n\n(ELN automatically saves your work every few seconds, so hopefully"
      " your data loss is minimal."
      " Regardless: apologies for the inconvenience.)";
  } else {
    if (!s.endsWith(".")) 
      s += ".";
    msg = "    " + s;
    msg += "\n\nPlease send a bug report to the author at " + email + "."
      "\n\n(ELN automatically saves your work every few seconds, so hopefully"
      " your data loss is minimal."
      " Regardless: apologies for the inconvenience.)";
  }
  mb.setInformativeText(msg);
  if (!bits.isEmpty())
    mb.setDetailedText(bits.join(CR_MARKER));
  mb.exec();
  exit(1);
}  

CrashReport::CrashReport() {
  if (pipe(eln_pipe)) {
    qDebug() << "pipe failed";
    return;
  }
  pid_t p = fork();
  if (p<0) {
    qDebug() << "fork failed";
    close(eln_pipe[0]);
    close(eln_pipe[1]);
    eln_pipe[0] = eln_pipe[1] = -1;
    return;
  } else if (p>0) {
    // parent
    close(eln_pipe[1]);
    close(0);
    eln_crashreporter(eln_pipe[0], Translate::_("author-email"));
    exit(1);
  } else {
    // child
    close(eln_pipe[0]);
    eln_grabsignals();
  }
}

CrashReport::~CrashReport() {
  // normal exit
}
  

#else

CrashReport::CrashReport() {
  // Crash reporting not yet implemented for Windows; I don't know
  // how to fork. Instead, I could run a new instance of eln with
  // some special flag ("eln -crashreporter") that would act as the
  // crash reporter.
}

CrashReport::~CrashReport() {
}

#endif
