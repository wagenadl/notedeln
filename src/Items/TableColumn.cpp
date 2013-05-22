// TableColumn.cpp

#include "TableColumn.H"

TableColumn::TableColumn(TableBlockItem *parent, int c): QObject(parent) {
  ccChangeMapper = new QSignalMapper(this);
  connect(ccChangeMapper, SIGNAL(mapped(int)), SLOT(reassessWidth(int)));
  regrabCells(c);
}

TableColumn::~TableColumn() {
}

TableBlockItem *TableColumn::tableItem() {
  TableBlockItem *tbi = dynamic_cast<TableBlockItem *>(parent());
  ASSERT(tbi);
  return tbi;
}

TableBlockData *TableColumn::tableData() {
  TableBlockData *tbd = tableItem()->data();
  ASSERT(tbd);
  return tbd;
}

TableCellItem *TableColumn::cell(int r) {
  ASSERT(r>=0 && r<cells.size());
  return cells[r];
}

void TableCellItem::setPos(double x) {
  for (int r=0; r<cells.size(); r++)
    cells[r]->setPos(x, 0);
}

void TableColumn::regrabCells(int c) {
  cidx = c;
  cells.clear();
  rformaxw = -1;
  TableBlockItem *tbi = tableItem();
  TableBlockData *tbd = tableData();
  for (int r=0; r<tbd->rows(); r++) 
    cells.append(tbi->row(r)->cell(c));
}

void TableColumn::reassessWidth(int r) {
  double w1 = cells[r]->fittedWidth();
  double w0 = tableData()->columnWidth(cidx);
  if (w1>w0) {
    tableData()->setColumnWidth(cidx, h1);
    rformaxw = r;
    emit widthChanged();
  } else if ((r==rformaxw || rformaxw<0) && w1<w0) {
    rformaxw = r; // needed if prev'ly <0
    for (int r1=0; r1<cells.size(); r1++) {
      if (r1==r)
        continue;
      double w2 = cells[r1]->fittedWidth();
      if (w2>w1) {
        w1 = w2;
        rformaxw = r1;
      }
    }
    if (w1!=w0) {
      tableData()->setColumnWidth(cidx, w1);
      emit widthChanged();
    }
  }
}
