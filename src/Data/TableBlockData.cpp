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
