// Calltrace.cpp

#include "Calltrace.h"

#ifdef Q_OS_LINUX
#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <QDebug>

namespace Calltrace {

  QStringList list() {
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
  
  QString quick() {
    QStringList fns = list();
    fns.pop_front();
    QString trc = fns.join(" < ");
    trc.replace(QRegExp(" <  <  < ( < )*"), " <...< ");
    trc.replace("  ", " ");
    return trc;
  }

  QString full(int skip) {
    QStringList fns = list();
    while (skip-- > 0) 
      fns.pop_front();
    QString trc = fns.join(" < ");
    trc.replace(QRegExp(" <  <  < ( < )*"), " <...< ");
    trc.replace("  ", " ");
    return trc;
  }

};

#else

namespace Calltrace {
  QStringList list() {
    return QStringList();
  }

  QString quick() {
    return "";
  }

  QString full(int) {
    return "";
  }
};

#endif
