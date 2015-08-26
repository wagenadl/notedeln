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

#include "Assert.h"
#include <QDebug>
#include "Calltrace.h"

#ifdef QT_NO_DEBUG
#define ASSERT_BACKTRACE 0
#else
#define ASSERT_BACKTRACE 1
#endif

Assertion::Assertion(QString msg, bool tts): msg(msg), trytosave(tts) {
#if ASSERT_BACKTRACE
  trc = Calltrace::full(1);
#endif
  qDebug() << trc;
  qDebug() << msg;
}

int &Assertion::priorFailures() {
  static int pf = 0;
  return pf;
}

void Assertion::crash(QString msg, char const *file, int line) {
  Assertion a(QString::fromUtf8("Assertion “%1” failed"
                                " in file “%2” at line %3.")
              .arg(msg).arg(file).arg(line),
              false);

  if (++priorFailures()>2) {
    qDebug() << "Assertion failed while quitting. Terminating hard.\n";
    abort();
  } else {
    throw a;
  }
}

void Assertion::saveThenCrash(QString msg, char const *file, int line) {
  Assertion a(QString::fromUtf8("Assertion “%1” failed"
                                " in file “%2” at line %3.")
              .arg(msg).arg(file).arg(line),
              true);

  if (++priorFailures()) {
    qDebug() << "Assertion failed while quitting. Terminating hard.\n";
    abort();
  } else {
    throw a;
  }
}
