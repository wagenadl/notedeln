// Data/TableCell.h - This file is part of NotedELN

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

// TableCell.h

#ifndef TABLECELL_H

#define TABLECELL_H

class TableCell {
public:
  TableCell(class TableData const *tbl=0, int r=-1, int c=-1):
    tbl(tbl), r(r), c(c) { }
  int row() const { return r; }
  int column() const { return c; }
  bool isValid() const;
  int firstPosition() const;
  int lastPosition() const;
  int length() const;
  bool isEmpty() const;
  bool operator==(TableCell const &a) const;
  bool operator!=(TableCell const &a) const { return !operator==(a); }
  TableData const *table() const { return tbl; }
private:
  TableData const *tbl;
  int r, c;
};

class TableCellRange {
public:
  TableCellRange(TableData const *tbl=0, int r0=-1, int c0=-1,
                 int nr=0, int nc=0):
    tbl(tbl), r0(r0), c0(c0), nr(nr), nc(nc) { }
  TableCellRange(TableCell start, TableCell end);
  bool isEmpty() const { return nr==0 || nc==0; }
  bool isSingleCell() const { return nr==1 && nc==1; }
  int firstRow() const { return r0; }
  int firstColumn() const { return c0; }
  int lastRow() const { return r0+nr-1; }
  int lastColumn() const { return c0+nc-1; }
  int rows() const { return nr; }
  int columns() const { return nc; }
  TableData const *data() const { return tbl; }
private:
  TableData const *tbl;
  int r0, c0, nr, nc;
};
  
#endif
