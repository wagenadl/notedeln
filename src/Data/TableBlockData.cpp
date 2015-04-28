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

#include "TableBlockData.h"
#include "TableData.h"
#include "Assert.h"
#include <QSet>
#include <QDebug>

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
  // now we must remove spurious "\n" and reorganize markups
  QSet<int> removedNewlines;
  QString s = tbd1->text()->text();
  for (int k=s.indexOf("\n"); k>=0; k=s.indexOf("\n", k+1))
    removedNewlines << k;
  s.remove("\n");
  tbd1->text()->setText(s);
  
  foreach (MarkupData *md, tbd1->text()->children<MarkupData>()) {
    int s0 = md->start();
    int e0 = md->end();
    int preStart=0;
    int preEnd=0;
    foreach (int split, removedNewlines) {
      if (s0>split)
	preStart++;
      if (e0>split)
	preEnd++;
    }
    md->setStart(s0-preStart);
    md->setEnd(e0-preEnd);
  }
  
  return tbd1;
}

TableBlockData *TableBlockData::deepCopyFromTextBlock(TextBlockData *tb,
						      int pos) {
  QString s = tb->text()->text();

  QVariantMap tbd0 = tb->save();
  tbd0["typ"] = "tableblock";
  QVariantList cc = tbd0["cc"].toList();
  for (int i=0; i<cc.size(); i++) {
    QVariantMap cm = cc[i].toMap();
    if (cm["typ"]=="text") {
      cm["typ"] = "table";
      QVariantList ll;
      ll.append(pos);
      ll.append(s.size()-pos);
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

  s.insert(pos, "\n");
  s.insert(0, "\n");
  s.append("\n");
  tbd1->text()->setText(s);

  foreach (MarkupData *md, tbd1->text()->children<MarkupData>()) {
    int s0 = md->start();
    int e0 = md->end();
    md->setStart(s0 + (s0>=pos ? 2 : 1));
    md->setEnd(e0 + (e0>pos ? 2 : 1));
  }
  return tbd1;
}

