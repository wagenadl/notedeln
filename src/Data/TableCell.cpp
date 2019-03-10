// Data/TableCell.cpp - This file is part of eln

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

// TableCell.cpp

#include "TableCell.h"
#include "TableData.h"
#include "ElnAssert.h"
#include <QDebug>

bool TableCell::isValid() const {
  if (tbl && c>=0 && r>=0
      && c<tbl->columns()
      && r<tbl->rows())
    return true;
  qDebug() << "TableCell::notValid" << tbl << r << c
           << "(" << tbl->rows() << tbl->columns() << ")";
  return false;
}

bool TableCell::operator==(TableCell const &a) const {
  return tbl==a.tbl && c==a.c && r==a.r;
}

int TableCell::firstPosition() const {
  ASSERT(isValid());
  return tbl->cellStart(r, c);
}

int TableCell::lastPosition() const {
  ASSERT(isValid());
  return tbl->cellStart(r, c) + tbl->cellLength(r, c);
}

bool TableCell::isEmpty() const {
  ASSERT(isValid());
  return tbl->cellLength(r,c)==0;
}

int TableCell::length() const {
  ASSERT(isValid());
  return tbl->cellLength(r,c);
}

TableCellRange::TableCellRange(TableCell start, TableCell end) {
  ASSERT(start.table() == end.table());
  r0 = start.row();
  c0 = start.column();
  int dr = end.row() - start.row();
  int dc = end.column() - start.column();
  if (dr<0) {
    r0 += dr;
    nr = 1 - dr;
  } else {
    nr = 1 + dr;
  }
  if (dc<0) {
    c0 += dc;
    nc = 1 - dc;
  } else {
    nc = 1 + dc;
  }
}
