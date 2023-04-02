// Items/Unicode.cpp - This file is part of NotedELN

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

// Unicode.cpp

#include "Unicode.h"

namespace Unicode {
  bool isCombining(QChar c) {
    char16_t u = c.unicode();
    return (u>=0x0300 && u<=0x036f)
      || (u>=0x1ab0 && u<=0x1aff)
      || (u>=0x1dc0 && u<=0x1dff)
      || (u>=0x20d0 && u<=0x20ff)
      || (u>=0xfe20 && u<=0xfe2f)
      || u==0x3099 || u==0x309a;
  }
};

