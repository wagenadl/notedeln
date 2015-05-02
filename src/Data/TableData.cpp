// Data/TableData.cpp - This file is part of eln

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

// TableData.C

#include "TableData.h"
#include "Assert.h"
#include <QDebug>

static Data::Creator<TableData> c("table");

TableData::TableData(Data *parent): TextData(parent) {
  setType("table");
  nc = nr = 2;
  text_ = "\n\n\n\n\n"; // one more than number of cells
  recalculate();
}

TableData::~TableData() {
}

void TableData::recalculate() {
  qDebug() << "TD::recalculate" << countCells() << nc << nr << text_;
  if (countCells() != nc*nr) {
    valid = false;
    return;
  }

  lengths.resize(nc*nr);
  starts.resize(nc*nr);
  int offset = 1;
  for (int n=0; n<nc*nr; n++) {
    int next = text_.indexOf("\n", offset);
    starts[n] = offset;
    lengths[n] = next - offset;
    offset = next + 1;
  }
  qDebug() << "TD::reclc" << starts << lengths;
  valid = true;
}

bool TableData::isEmpty() const {
  return false;
}

int TableData::rows() const {
  return nr;
}

int TableData::columns() const {
  return nc;
}

void TableData::setRows(int r) {
  if (nr==r)
    return;
  nr = r;
  recalculate();
  if (valid) 
    markModified();
  else
    qDebug() << "TableData::setRows: Not saving: inconsistent count";
}

void TableData::setColumns(int c) {
  if (nc==c)
    return;
  nc = c;
  recalculate();
  if (valid) 
    markModified();
  else
    qDebug() << "TableData::setColumns: Not saving: inconsistent count";
}

void TableData::setText(QString const &t, bool hushhush) {
  qDebug() << "TableData::setText" << t << hushhush;
  if (text_ == t)
    return;
  TextData::setText(t, true);
  recalculate();
  if (valid) {
    if (!hushhush)
      markModified();
  } else {
    qDebug() << "TableData::setText: Not saving: inconsistent count";
  }
}    
  
    

bool TableData::isValid() const {
  return valid;
}

int TableData::countCells() const {
  int count = 0;
  int offset = 1;
  while (true) {
    int next = text_.indexOf("\n", offset);
    if (next<0)
      return count;
    offset = next+1;
    count++;
  }
}

int TableData::rc2index(int r, int c) const {
  ASSERT(valid);
  ASSERT(r>=0 && r<nr && c>=0 && c<nc);
  return r*nc+c;
}


int TableData::cellLength(int r, int c) const {
  return lengths[rc2index(r, c)];
}

int TableData::cellStart(int r, int c) const {
  return starts[rc2index(r, c) ];
}

int TableData::cellEnd(int r, int c) const {
  return cellStart(r, c) + cellEnd(r, c);
}

QString TableData::cellContents(int r, int c) const {
  return text().mid(cellStart(r, c), cellLength(r, c));
}

bool TableData::isColumnEmpty(int c) const {
  for (int r=0; r<nr; r++)
    if (!isCellEmpty(r,c))
      return false;
  return true;
}
  
bool TableData::isRowEmpty(int r) const {
  for (int c=0; c<nc; c++)
    if (!isCellEmpty(r,c))
      return false;
  return true;
}

bool TableData::isCellEmpty(int r, int c) const {
  return cellLength(r,c)==0;
}


int TableData::lastNonEmptyCellInRow(int r) const {
  for (int c=nc-1; c>0; c--)
    if (!isCellEmpty(r,c))
      return c;
  return 0;
}

void TableData::loadMore(QVariantMap const &) {
  // We don't load cell lengths any more: they are trivial to recalculate
  recalculate();
}

void TableData::saveMore(QVariantMap &dst) const {
  // We do save cell lengths still, for compatibility with earlier versions
  QVariantList xl;
  foreach (int l, lengths)
    xl.append(QVariant(l));
  dst["len"] = QVariant(xl);
}

void TableData::setLineStarts(QVector<int> const &) {
  qDebug() << "TableData does not use line starts. setLineStarts ignored.";
}

QVector<int> const &TableData::lineStarts() const {
  return starts;
}
  
TableCell TableData::cellAt(int pos) const {
  for (int r=0; r<nr; r++) {
    for (int c=0; c<nc; c++) {
      TableCell cel(this, r, c);
      if (pos>=cel.firstPosition() && pos<=cel.lastPosition())
        return cel;
    }
  }
  return TableCell();
}

TableCell TableData::cell(int row, int col) const {
  return TableCell(this, row, col);
}
