// TableRowItem.cpp

#include "TableRowItem.H"

#include "TableBlockItem.H"
#include "TableCellItem.H"
#include "TableBlockData.H"

#include <QSignalMapper>

TableRowItem::TableRowItem(TableRowData *data, Item *parent):
  Item(data, parent) {

  ccChangeMapper = new QSignalMapper(this);
  connect(ccChangeMapper, SIGNAL(mapped(int)), SLOT(reassessHeight(int)));
  cformaxh = -1;
  makeCells();
}

void TableRowItem::makeCells() {
  for (int c=0; c<data()->cells(); c++) {
    TableCellItem *tci = new TableCellItem(data()->cell(c), this);
    connect(tci, SIGNAL(textChange()), ccChangeMapper, SLOT(map()));
    cells.append(tci);
    fittedWidths.append(tci->fittedWidth());
  }
  remap();
}

void TableRowItem::remap() {
  for (int c=0; c<cells.size(); c++) {
    ccChangeMapper->setMapping(cells[c], c);
  }
}

void TableRowItem::reassessHeight(int c) {
  double h1 = cells[c]->fittedHeight();
  double h0 = data()->height();
  if (h1>h0) {
    data()->setHeight(h1);
    cformaxh = c;
    emit heightChanged();
  } else if ((c==cformaxh || cformaxh<0) && h1<h0) {
    cformaxh = c; // needed if prev'ly <0
    for (int c1=0; c1<cells.size(); c1++) {
      if (c1==c)
        continue;
      double h2 = cells[c1]->fittedHeight();
      if (h2>h1) {
        h1 = h2;
        cformaxh = c1;
      }
    }
    if (h1!=h0) {
      data()->setHeight(h1);
      emit heightChanged();
    }
  }
}
