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

#include <QObject>
#include <QStringList>

void eln_backtrace(int skip=0);

QStringList eln_calltrace();
QString eln_quickcalltrace();

void eln_grabsignals();
void eln_ungrabsignals();

void eln_assert(char const *assertion, char const *file, int line);

inline void eln_noop() { }

#define ASSERT(cond) ((!(cond)) ? eln_assert(#cond, __FILE__, __LINE__) : eln_noop())

#define ASSERTX(cond, str) ((!(cond)) ? eln_assert(str, __FILE__, __LINE__) : eln_noop())

#endif
