// SignalGrabber.cpp

#include "SignalGrabber.h"

#ifdef Q_OS_LINUX

#include "Calltrace.h"
#include <stdlib.h>
#include <stdio.h>

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
  
  fprintf(stderr, "Signal %i caught.\n", sig);
  QString trace = Calltrace::quick();
  fprintf(stderr, "Backtrace: %s\n", trace.toLatin1().constData());
  fprintf(stderr, "The application, unfortunately, must quit immediately.\n");
  fprintf(stderr, "Please file a bug report.\n");
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

SignalGrabber::SignalGrabber() {
  eln_grabsignals();
}

SignalGrabber::~SignalGrabber() {
  eln_ungrabsignals();
}

#else

SignalGrabber::SignalGrabber() {
}

SignalGrabber::~SignalGrabber() {
}

#endif
