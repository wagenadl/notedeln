// Scenes/Roman.cpp - This file is part of eln

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

// Roman.C

#include "Roman.H"

Roman::Roman(int n): n(n) {
  s = "";
  build(1000, "m");
  build(900, "cm");
  build(500, "d");
  build(400, "cd");
  build(100, "c");
  build(90, "xc");
  build(50, "l");
  build(40, "xl");
  build(10, "x");
  build(9, "ix");
  build(5, "v");
  build(4, "iv");
  build(1, "i");
}

QString Roman::lc() const {
  return s;
}

QString Roman::uc() const {
  return s.toUpper();
}

void Roman::build(int dn, QString c) {
  while (n>=dn) {
    n-=dn;
    s+=c;
  }
}
