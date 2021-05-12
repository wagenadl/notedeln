// Data/UUID.cpp - This file is part of NotedELN

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

// UUID.cpp

#include "UUID.h"
#include "Random.h"

QString UUID::create(int bits) {
  QString res;
  for (int k=0; k<bits; k+=16) {
    int x = Random::random();
    for (int l=0; l<16; l+=4) {
      if (k+l >= bits)
	break;
      int y = x & 15;
      if (y<10)
	res += ('0' + y);
      else
	res += ('a' + y - 10);
      x >>= 4;
    }
  }
  return res;
}
