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

#include "CrashReport.h"

#if defined(Q_OS_UNIX) || defined(Q_OS_MAC)

#include <unistd.h>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <stdio.h>
#include <signal.h>
#include "Calltrace.h"

static int eln_pipe[2] = { -1, -1 };

#define CR_MARKER "---"

static void eln_ungrabsignals() {
  struct sigaction newHdlr;
  newHdlr.sa_handler = SIG_DFL;
  sigemptyset(&newHdlr.sa_mask);
  newHdlr.sa_flags = SA_RESETHAND;
  sigaction(SIGSEGV, &newHdlr, 0);
  sigaction(SIGABRT, &newHdlr, 0);
}

static void eln_sighandler(int sig) {
  eln_ungrabsignals();
  int fd = eln_pipe[1];
  if (fd<0)
    fd = 2; // i.e., stderr
  char buf[100];
  int n = 0;
  sprintf(buf, "Signal %i caught.", sig);
  n = write(fd, buf, strlen(buf));

  sprintf(buf, CR_MARKER "Backtrace: ");
  n = write(fd, buf, strlen(buf));
  QString trace = Calltrace::quick();
  char const *b = trace.toLatin1().constData();
  n = write(fd, b, strlen(b));

  close(fd);
  if (n) { }
  exit(128+sig);
}

static void eln_grabsignals() {
  struct sigaction newHdlr;
  newHdlr.sa_handler = &eln_sighandler;
  sigemptyset(&newHdlr.sa_mask);
  newHdlr.sa_flags = SA_RESETHAND;
  sigaction(SIGSEGV, &newHdlr, 0);
  sigaction(SIGABRT, &newHdlr, 0);
}

static void eln_crashreporter(int fd) {
  QString in;
  char buf[1024];
  int n;
  while (true) {
    n = read(fd, buf, 1023);
    if (n<=0)
      break;
    buf[n] = 0;
    write(2, buf, n);
    in += buf;
  }

  if (in.isEmpty()) 
    exit(0);

  QStringList bits = in.split(CR_MARKER);
  
  int argc = 1;
  char arg0[] = "eln_crash";
  char *argv[] = { arg0, 0 };
  QApplication app(argc, argv);

  QMessageBox mb(QMessageBox::Critical, "eln",
                 "eln suffered a fatal internal error and had to close:",
                 QMessageBox::Close);
  QString s = bits.takeFirst().trimmed();
  if (!s.endsWith("."))
    s += ".";
  mb.setInformativeText(s + "\n\nPlease send a bug report to the author.");
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
  } else if (p==0) {
    // child
    close(eln_pipe[1]);
    close(0);
    eln_crashreporter(eln_pipe[0]);
    exit(1);
  } else {
    // parent
    close(eln_pipe[0]);
    eln_grabsignals();
  }
}

#else

CrashReport::CrashReport() {
}

#endif
