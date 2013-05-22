// TableCellData.cpp

#include "TableCellData.H"
#include "TableRowData.H"
#include "Assert.H"

static Data::Creator<TableCellData> c("cell");

TableCellData::TableCellData(Data *parent): TextData(parent) {
  setType("cell");
}

TableCellData::~TableCellData() {
}

TableRowData *TableCellData::row() const {
  TableRowData *trd = dynamic_cast<TableRowData *>(parent());
  ASSERT(trd);
  return trd;
}

TableBlockData *TableCellData::table() const {
  return row()->table();
}


void TableCellData::setColumnIndex(int c) {
  cidx = c;
}

int TableCellData::columnIndex() const {
  return cidx;
}
