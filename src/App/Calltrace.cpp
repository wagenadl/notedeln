// App/Calltrace.cpp - This file is part of NotedELN

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

// Calltrace.cpp

#include "Calltrace.h"
#include <QRegularExpression>

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)

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
    trc.replace(QRegularExpression(" <  <  < ( < )*"), " <...< ");
    trc.replace("  ", " ");
    return trc;
  }

  QString full(int skip) {
    QStringList fns = list();
    while (skip-- > 0) 
      fns.pop_front();
    QString trc = fns.join(" < ");
    trc.replace(QRegularExpression(" <  <  < ( < )*"), " <...< ");
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
