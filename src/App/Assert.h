// App/Assert.H - This file is part of eln

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

// Assert.H

#ifndef ASSERT_H

#define ASSERT_H

#include <QString>

class Assertion {
public:
  Assertion(QString msg, bool trytosave);
  QString message() const { return msg; }
  QString backtrace() const { return trc; }
  bool shouldSave() const { return trytosave; }
public:
  static void crash(QString msg, char const *file=0, int line=0);
  static void saveThenCrash(QString msg, char const *file=0, int line=0);
  inline static void noOp() { }
private:
  static int &priorFailures();
private:
  QString msg;
  QString trc;
  bool trytosave;
};

#define ASSERT(cond) ((!(cond)) ? Assertion::crash(#cond, __FILE__, __LINE__) : Assertion::noOp())

#define ASSERTSAVE(cond) ((!(cond)) ? Assertion::saveThenCrash(#cond, __FILE__, __LINE__) : Assertion::noOp())


#endif
