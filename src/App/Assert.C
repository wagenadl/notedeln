// Assert.C

#include "Assert.H"
#include "App.H"
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>

#define ASSERT_BACKTRACE 0

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

void eln_sighandler(int sig) {
  fprintf(stderr, "Signal %i caught.\n", sig);

  QString trc = eln_quickcalltrace();
  fprintf(stderr, "Backtrace: %s\n", trc.toLatin1().constData());

  fprintf(stderr, "Shutting down application.\n");
  App::quit();
  fprintf(stderr, "Application shut down. Exiting.\n");
  exit(1);
}

void eln_grabsignals() {
  struct sigaction newHdlr;
  newHdlr.sa_handler = &eln_sighandler;
  sigemptyset(&newHdlr.sa_mask);
  newHdlr.sa_flags = SA_RESETHAND;
  sigaction(SIGSEGV, &newHdlr, 0);
  sigaction(SIGABRT, &newHdlr, 0);
}
