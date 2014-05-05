// Items/TableBlockItem.cpp - This file is part of eln

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

// TableBlockItem.cpp

#include "TableBlockItem.H"
#include "TableItem.H"

TTICreator::~TTICreator() {
}

TextItem *TTICreator::create(TextData *data, Item *parent,
			     QTextDocument *altdoc) const {
  TableData *d = dynamic_cast<TableData*>(data);
  ASSERT(d);
  ASSERT(altdoc==0);
  return new TableItem(d, parent);
}

TableBlockItem::TableBlockItem(TableBlockData *data, Item *parent):
  TextBlockItem(data, parent, TTICreator()) {
  item_ = firstChild<TableItem>();
  ASSERT(item_);
  connect(item_, SIGNAL(unicellular(TableData*)),
	  this, SIGNAL(unicellular(TableData*)));

  QTextCursor tc(item_->document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setTextIndent(0);
  fmt.setLineHeight(100,
		    QTextBlockFormat::ProportionalHeight);
  tc.movePosition(QTextCursor::Start);
  tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  tc.setBlockFormat(fmt);
}

TableBlockItem::~TableBlockItem() {
}


TableItem *TableBlockItem::table() {
  return item_;
}
