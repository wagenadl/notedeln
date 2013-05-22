// TableBlockItem.cpp

#include "TableBlockItem.H"
#include "TableItem.H"

TextItem *TTICreator::create(TextData *data, Item *parent) const {
  TableData *d = dynamic_cast<TableData*>(data);
  ASSERT(d);
  return new TableItem(d, parent);
}

TableBlockItem::TableBlockItem(TableBlockData *data, Item *parent):
  TextBlockItem(data, parent, TTICreator()) {
  item_ = firstChild<TableItem>();
  ASSERT(item_);
}

TableBlockItem::~TableBlockItem() {
}


TableItem *TableBlockItem::table() {
  return item_;
}
