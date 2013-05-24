// Data/TableBlockData.cpp - This file is part of eln

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

// TableBlockData.cpp

#include "TableBlockData.H"
#include "TableData.H"
#include "Assert.H"

static Data::Creator<TableBlockData> c("tableblock");

TableBlockData::TableBlockData(Data *parent): TextBlockData(parent) {
  setType("tableblock");
  deleteChild(text_);
  table_ = new TableData(this);
  text_ = table_;
}


TableBlockData::~TableBlockData() {
}

TableData const *TableBlockData::table() const {
  return table_;
}

TableData *TableBlockData::table() {
  return table_;
}

void TableBlockData::loadMore(QVariantMap const &src) {
  TextBlockData::loadMore(src);
  // Our old table_ pointer is now invalidated: since it was part of
  // Data's children array, it has already been deleted.
  table_ = firstChild<TableData>();
  ASSERT(table_);
}

bool TableBlockData::isEmpty() const {
  return BlockData::isEmpty() && table_->isEmpty();
}
