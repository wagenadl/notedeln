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

TextBlockData *TableBlockData::deepCopyAsTextBlock() const {
  QVariantMap tbd0 = save();
  tbd0["typ"] = "textblock";
  QVariantList cc = tbd0["cc"].toList();
  for (int i=0; i<cc.size(); i++) {
    QVariantMap cm = cc[i].toMap();
    if (cm["typ"]=="table") {
      cm["typ"] = "text";
      cm.remove("len");
      cc[i] = cm;
      break;
    }
  }
  tbd0["cc"] = cc;
  TextBlockData *tbd1 = new TextBlockData();
  tbd1->load(tbd0);
  return tbd1;
}

TableBlockData *TableBlockData::deepCopyFromTextBlock(TextBlockData *tb) {
  QString t0 = tb->text()->text();
  if (!t0.startsWith("\n"))
    return 0;
  int idx = t0.indexOf("\n", 1);
  if (idx<0)
    return 0;
  if (t0.indexOf("\n", idx+1) != t0.length()-1)
    return 0;
  
  QVariantMap tbd0 = tb->save();
  tbd0["typ"] = "tableblock";
  QVariantList cc = tbd0["cc"].toList();
  for (int i=0; i<cc.size(); i++) {
    QVariantMap cm = cc[i].toMap();
    if (cm["typ"]=="text") {
      cm["typ"] = "table";
      QVariantList ll;
      ll.append(idx-1);
      ll.append(t0.length()-idx-2);
      cm["len"] = ll;
      cm["nc"] = 2;
      cm["nr"] = 1;
      cc[i] = cm;
      break;
    }
  }
  tbd0["cc"] = cc;
  TableBlockData *tbd1 = new TableBlockData();
  tbd1->load(tbd0);
  return tbd1;
}

