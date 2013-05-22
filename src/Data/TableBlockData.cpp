// TableBlockData.cpp

#include "TableBlockData.H"
#include "TableRowData.H"
#include "Assert.H"

static Data::Creator<TableBlockData> c("table");

TableBlockData::TableBlockData(Data *parent): BlockData(parent) {
  setType("table");
  nrows = 1;
  ncols = 1;
  rh.append(0);
  cw.append(0);
}

TableBlockData::~TableBlockData() {
}

int TableBlockData::rows() const {
  return nrows;
}

int TableBlockData::columns() const {
  return ncols;
}

QList<double> const &TableBlockData::rowHeights() const {
  return rh;
}

QList<double> const &TableBlockData::columnWidths() const {
  return cw;
}

double TableBlockData::rowHeight(int r) const {
  ASSERT(r>=0 && r<nrows);
  return rh[r];
}

double TableBlockData::columnWidth(int c) const {
  ASSERT(c>=0 && c<ncols);
  return cw[c];
}

void TableBlockData::setRowHeight(int r, double h) {
  ASSERT(r>=0 && r<nrows);
  rh[r] = h;
  markModified();
}

void TableBlockData::setColumnWidth(int c, double w) {
  ASSERT(c>=0 && c<ncols);
  cw[c] = w;
  markModified();
}

void TableBlockData::setRows(int R) {
  nrows = R;
  QList<TableRowData*> rr = children<TableRowData>();
  while (rr.size()>nrows) 
    deleteChild(rr.takeLast(), InternalMod);
  while (rh.size()>nrows)
    rh.takeLast();
  markModified();
}

void TableBlockData::setColumns(int C) {
  ncols = C;
  foreach (TableRowData *r, children<TableRowData>()) 
    r->setCells(ncols);
  while (cw.size()>ncols)
    cw.takeLast();
  markModified();
}

TableRowData *TableBlockData::row(int r) {
  ASSERT(r>=0 && r<nrows);
  // This is where we have to add rows if we have not enough!
  QList<TableRowData*> rr = children<TableRowData>();
  int dr = r - rr.size();
  while (dr>=0) {
    // have to invent some extra rows
    TableRowData *trd = new TableRowData(this);
    trd->setRowIndex(rr.size());
    trd->setCells(ncols);
    rr.append(trd);
    dr--;
  }
  return rr[r];
}

void TableBlockData::insertRowBefore(int r) {
  if (r==nrows) {
    addRow();
  } else {
    ASSERT(r>=0 && r<nrows);
    TableRowData *trd0 = row(r); // this is the row before which we will insert
    // (it had better be initialized!)
    TableRowData *trd = new TableRowData(0);
    trd->setCells(ncols);
    insertChildBefore(trd, trd0, InternalMod);
    rh.insert(r, 0);
    nrows++;
    fixRowIndices();
    markModified();
  }
}

void TableBlockData::addRow() {
  // empty cells are not initialized until they are referenced
  rh.append(0);
  nrows++;
  markModified();
}

void TableBlockData::deleteRow(int r) {
  QList<TableRowData *> rr = children<TableRowData>();
  if (rr.size()>r)
    deleteChild(rr[r], InternalMod); // only delete if initialized
  rh.removeAt(r);
  nrows--;
  fixRowIndices();
  markModified();  
}

void TableBlockData::fixRowIndices() {
  int r = 0;
  foreach (TableRowData *trd, children<TableRowData>()) 
    trd->setRowIndex(r++);
}  

void TableBlockData::insertColumnBefore(int c) {
  foreach (TableRowData *r, children<TableRowData>())
    r->insertCellBefore(c);
  cw.insert(c, 0);
  ncols++;
  markModified();
}

void TableBlockData::addColumn() {
  foreach (TableRowData *r, children<TableRowData>())
    r->addCell();
  cw.append(0);
  ncols++;
  markModified();
}

void TableBlockData::deleteColumn(int c) {
  foreach (TableRowData *r, children<TableRowData>())
    r->deleteCell(c);
  cw.removeAt(c);
  ncols--;
  markModified();
}


void TableBlockData::loadMore(QVariantMap const &src) {
  rh.clear();
  cw.clear();

  foreach (QVariant v, src["rh"].toList())
    rh.append(v.toDouble());
  foreach (QVariant v, src["cw"].toList())
    cw.append(v.toDouble());

  ASSERT(rh.size() == nrows);
  ASSERT(cw.size() == ncols);
}

void TableBlockData::saveMore(QVariantMap &dst) const {
  QVariantList rhl;
  foreach (double v, rh)
    rhl.append(QVariant(v));
  dst["rh"] = QVariant(rhl);

  QVariantList cwl;
  foreach (double v, cw)
    cwl.append(QVariant(v));
  dst["cw"] = QVariant(cwl);
}

