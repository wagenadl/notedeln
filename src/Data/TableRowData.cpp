// TableRowData.cpp

#include "TableRowData.H"
#include "TableCellData.H"
#include "TableBlockData.H"
#include "Assert.H"

static Data::Creator<TableRowData> c("row");

TableRowData::TableRowData(Data *parent): Data(parent) {
  setType("row");
  ncells = 0;
}

TableRowData::~TableRowData() {
}

int TableRowData::cells() const {
  return ncells;
}

void TableRowData::setCells(int C) {
  ncells = C;
  QList<TableCellData*> cc = children<TableCellData>();
  while (cc.size()>ncells) 
    deleteChild(cc.takeLast(), InternalMod);
  markModified();
}

TableCellData *TableRowData::cell(int c) {
  ASSERT(c>=0 && c<ncells);
  QList<TableCellData*> cc = children<TableCellData>();
  int dc = c - cc.size();
  while (dc>=0) {
    // have to invent some extra cells
    TableCellData *tcd = new TableCellData(this);
    tcd->setCellIndex(cc.size());
    cc.append(tcd);
    dc--;
  }
  return cc[c];
}

void TableRowData::insertCellBefore(int c) {
  if (c==ncells) {
    addCell();
  } else {
    ASSERT(c>=0 && c<ncells);
    TableCellData *tcd0 = cell(c); // this is the cell before which we will
    // insert (it had better be initialized!)
    TableCellData *tcd = new TableCellData(0);
    insertChildBefore(tcd, tcd0, InternalMod);
    ncells++;
    fixCellIndices();
    markModified();
  }
}

void TableRowData::addCell() {
  // empty cells are not initialized until they are referenced
  ncells++;
  markModified();
}

void TableRowData::deleteCell(int c) {
  QList<TableCellData *> cc = children<TableCellData>();
  if (cc.size()>c)
    deleteChild(cc[c], InternalMod); // only delete if initialized
  ncells--;
  fixCellIndices();
  markModified();  
}

void TableRowData::fixCellIndices() {
  int c = 0;
  foreach (TableCellData *tcd, children<TableCellData>()) 
    tcd->setCellIndex(c++);
}

void TableRowData::setRowIndex(int r) {
  ridx = r;
}

int TableRowData::rowIndex() const {
  return ridx;
}  

double TableRowData::height() const {
  return table()->rowHeight(ridx);
}

void TableRowData::setHeight(double h) {
  table()->setRowHeight(ridx, h);
}

TableBlockData *TableRowData::table() const {
  TableBlockData *tbd = dynamic_cast<TableBlockData *>(parent());
  ASSERT(tbd);
  return tbd;
}
