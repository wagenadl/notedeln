// App/Assert.cpp - This file is part of eln

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

// Assert.C

#include "Assert.H"
#include "App.H"
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>

#ifdef Q_OS_LINUX
#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>
#endif

#ifdef QT_NO_DEBUG
#define ASSERT_BACKTRACE 0
#else
#define ASSERT_BACKTRACE 1
#endif

#ifdef Q_OS_LINUX
QStringList eln_calltrace() {
  QStringList fns;
  void *backtrace_data[1024];
  int backtrace_count = backtrace(backtrace_data, 1024);
  char **symbols = backtrace_symbols(backtrace_data, backtrace_count);
  size_t dm_length = 1024;
  char demangled[1024];
  for (int k=1; k<backtrace_count; k++) {
    int status;
    char *begin_name=0, *begin_offset=0, *end_offset=0;
    for (char *p=symbols[k]; *p; ++p) {
      if (*p=='(')
	begin_name=p;
      else if (*p=='+')
	begin_offset=p;
      else if (*p==')' && begin_offset && !end_offset)
	end_offset=p;
    }
    if (begin_name && begin_offset && end_offset
	&& begin_name<begin_offset) {
      *begin_name++ = *begin_offset++ = *end_offset = 0;
      abi::__cxa_demangle(begin_name, demangled, &dm_length, &status);
      if (status==0) 
        fns << demangled;
      else 
        fns << begin_name;
    } else {
      fns << ""; // don't try to use a useless symbol
    }
  }

  free(symbols);
  return fns;
}

QString eln_quickcalltrace() {
  QStringList fns = eln_calltrace();
  fns.pop_front();
  QString trc = fns.join(" < ");
  trc.replace(QRegExp(" <  <  < ( < )*"), " <...< ");
  trc.replace("  ", " ");
  return trc;
}
#else
QStringList eln_calltrace() {
  return QStringList();
}
QString eln_quickcalltrace() {
  return QString();
}
#endif

#if ASSERT_BACKTRACE
void eln_backtrace(int skip) {
  QStringList fns = eln_calltrace();
  while (skip>0)
    fns.pop_front();
  QString trc = fns.join(" < ");
  trc.replace(QRegExp(" <  <  < ( < )*"), " <...< ");
  trc.replace("  ", " ");
  qDebug() << trc;
}
#else
void eln_backtrace(int /* skip */) {
}
#endif

void eln_assert(char const *assertion, char const *file, int line) {
  static bool prior_failure = false;

  eln_backtrace(1);
  fprintf(stderr, "\n");
  fprintf(stderr, "Assertion \"%s\" failed in file \"%s\" line %i.\n",
	  assertion, file, line);

  if (prior_failure) {
    fprintf(stderr, "Assertion failed while quitting. Terminating hard.\n");
    abort();
  } else {
    prior_failure = true;
    App::quit();
    fprintf(stderr, "Application shut down OK. Terminating.\n");
    abort();
  }
}

#ifdef Q_OS_LINUX
void eln_sighandler(int sig) {
  fprintf(stderr, "Signal %i caught.\n", sig);

  QString trc = eln_quickcalltrace();
  fprintf(stderr, "Backtrace: %s\n", trc.toLatin1().constData());

  fprintf(stderr, "Shutting down application.\n");
  App::quit();
  fprintf(stderr, "Application shut down. Aborting.\n");
  eln_ungrabsignals();
  abort();
}

void eln_grabsignals() {
  struct sigaction newHdlr;
  newHdlr.sa_handler = &eln_sighandler;
  sigemptyset(&newHdlr.sa_mask);
  newHdlr.sa_flags = SA_RESETHAND;
  sigaction(SIGSEGV, &newHdlr, 0);
  sigaction(SIGABRT, &newHdlr, 0);
}

void eln_ungrabsignals() {
  struct sigaction newHdlr;
  newHdlr.sa_handler = SIG_DFL;
  sigemptyset(&newHdlr.sa_mask);
  newHdlr.sa_flags = SA_RESETHAND;
  sigaction(SIGSEGV, &newHdlr, 0);
  sigaction(SIGABRT, &newHdlr, 0);
}

#else
void eln_grabsignals() {
}

void eln_ungrabsignals() {
}
#endif

