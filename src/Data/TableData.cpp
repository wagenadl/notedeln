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

#include "TableData.H"
#include "Assert.H"

static Data::Creator<TableData> c("table");

TableData::TableData(Data *parent): TextData(parent) {
  setType("table");
  nc = nr = 2;
  // in actual practice, this will usu. be overwritten soon
  lengths.resize(nr*nc);
  starts.resize(nr*nc);
  text_ = "\n\n\n\n\n"; // one more than number of cells
  firstInvalidStart = 0;
}

TableData::~TableData() {
}

bool TableData::isEmpty() const {
  return false;
}

unsigned int TableData::rows() const {
  return nr;
}

unsigned int TableData::columns() const {
  return nc;
}

void TableData::setRows(unsigned int r) {
  if (nr==r)
    return;
  nr = r;
  lengths.resize(nr*nc);
  starts.resize(nr*nc);
  firstInvalidStart = 0;
  markModified();
}

void TableData::setColumns(unsigned int c) {
  if (nc==c)
    return;
  nc = c;
  lengths.resize(nr*nc);
  starts.resize(nr*nc);
  firstInvalidStart = 0;
  markModified();
}

unsigned int TableData::cellLength(unsigned int r, unsigned int c) const {
  ASSERT(r<nr && c<nc);
  return lengths[rc2index(r, c)];
}

void TableData::setCellLength(unsigned int r, unsigned int c,
			      unsigned int len,
			      bool hushhush) {
  ASSERT(r<nr && c<nc);
  unsigned int idx = rc2index(r, c);
  if (lengths[idx]==len)
    return;
  lengths[idx] = len;
  if (firstInvalidStart > idx)
    firstInvalidStart = idx + 1;
  if (!hushhush)
    markModified();
}

unsigned int TableData::cellStart(unsigned int r, unsigned int c) const {
  ASSERT(r<nr && c<nc);
  unsigned int idx = rc2index(r, c);
  if (firstInvalidStart==0) {
    starts[0] = 1;
    firstInvalidStart = 1;
  }
  while (firstInvalidStart<=idx) {
    starts[firstInvalidStart] = starts[firstInvalidStart-1]
      + lengths[firstInvalidStart-1] + 1;
    firstInvalidStart++;
  }
  return starts[idx];
}

QString TableData::cellContents(unsigned int r, unsigned int c) const {
  ASSERT(r<nr && c<nc);
  return text().mid(cellStart(r, c), cellLength(r, c));
}

void TableData::loadMore(QVariantMap const &src) {
  lengths.clear();
  foreach (QVariant v, src["len"].toList())
    lengths.append(v.toInt());
  starts.resize(lengths.size());
  firstInvalidStart = 0;
}

void TableData::saveMore(QVariantMap &dst) const {
  QVariantList xl;
  foreach (unsigned int l, lengths)
    xl.append(QVariant(l));
  dst["len"] = QVariant(xl);
}

