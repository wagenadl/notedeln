// TableBlockData.cpp

#include "TableBlockData.H"

static Data::Creator<TableBlockData> c("table");

TableBlockData::TableBlockData(Data *parent): BlockData(parent) {
  setType("table");
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

void TableBlockData::setRows(int R) {
  nrows = R;
  QList<TableRowData*> rr = children<TableRowData>();
  while (rr.size()>nrows)
    deleteChild(rr.takeLast(), InternalMod);
  markModified();
  int dr = nrows-rr.size();
  while (dr>0) {
    TableRowData *tr = new TableRowData(this);
    tr->setCells(ncols);
    dr--;
  }
}


void GfxPointsData::loadMore(QVariantMap const &src) {
  xx_.clear();
  yy_.clear();

  foreach (QVariant v, src["xx"].toList())
    xx_.append(v.toDouble());
  foreach (QVariant v, src["yy"].toList())
    yy_.append(v.toDouble());
}

void GfxPointsData::saveMore(QVariantMap &dst) const {
  QVariantList xl;
  foreach (double v, xx_)
    xl.append(QVariant(v));
  dst["xx"] = QVariant(xl);

  QVariantList yl;
  foreach (double v, yy_)
    yl.append(QVariant(v));
  dst["yy"] = QVariant(yl);
}
  
  
