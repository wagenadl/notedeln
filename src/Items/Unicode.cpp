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
    return (c>=0x0300 && c<=0x036f)
      || (c>=0x1ab0 && c<=0x1aff)
      || (c>=0x1dc0 && c<=0x1dff)
      || (c>=0x20d0 && c<=0x20ff)
      || (c>=0xfe20 && c<=0xfe2f)
      || c==0x3099 || c==0x309a;
  }
};

