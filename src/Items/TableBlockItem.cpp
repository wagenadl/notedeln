// TableBlockItem.cpp

#include "TableBlockItem.H"
#include "TableRowItem.H"
#include "TableCellItem.H"

TableBlockItem::TableBlockItem(TableBlockData *data, Item *parent):
  BlockItem(data, parent) {
  setPos(style().real("margin-left"), 0);
  makeRows();
  makeColumns();
  restackRows();
  if (data()->height()==0)
    sizeToFit(); // is this correct behavior?
}

TableBlockItem::~TableBlockItem() {
}

void TableBlockItem::makeRows() {
  for (int r=0; r<data()->rows(); r++) {
    TableRowItem *tri = new TableRowItem(data()->row(r), this);
    connect(tri, SIGNAL(heightChanged()), SLOT(restackRows()));
    connect(tri, SIGNAL(heightChanged()), SLOT(sizeToFit()));
  }
}

void TableBlockItem::makeColumns() {
  for (int c=0; c<data()->columns(); c++) {
    TableColumn *tc = new TableColumn(this);
    connect(tc, SIGNAL(widthChanged(), SLOT(restackColumns())));
  }
}

void TableBlockItem::restackColumns() {
  double hpadding = style()->real("table-cell-hpadding");
  double x = hpadding;
  for (int c=0; c<data()->columns(); c++) {
    cols[c]->setPos(x);
    x += data().cellWidth(c);
    x += hpadding;
  }
}

void TableBlockItem::restackRows() {
  double vpadding = style()->real("table-cell-vpadding");
  double y = vpadding;
  QList<TableRowItem *> rr = children<TableRowItem>();
  for (int r=0; r<rr.size(); r++) {
    rr[r]->setPos(0, y);
    y += data().rowHeight(r);
    y += vpadding;
  }
}

void TableBlockItem::sizeToFit() {
  double vpadding = style()->real("table-cell-vpadding");
  double y = vpadding;
  for (int r=0; r<data().rows(); r++) {
    y += data().rowHeight(r);
    y += vpadding;
  }
  if (y != data()->height()) {
    data()->setHeight(y);
    emit heightChanged();
  }
}

void TableBlockItem::paint(QPainter *p,
                           const QStyleOptionGraphicsItem *, QWidget *) {
  double vpadding = style()->real("table-cell-vpadding");
  double hpadding = style()->real("table-cell-hpadding");
  QRectF bbox = boundingRect();
  bbox.adjust(hpadding/2, vpadding/2, -hpadding/2, -vpadding/2);

  QColor c(style().color("canvas-grid-color"));
  c.setAlphaF(style().real("canvas-grid-alpha"));
  p->setPen(QPen(c,
		 style().real("canvas-grid-line-width"),
		 Qt::SolidLine,
		 Qt::FlatCap));
  double y = vpadding/2;
  for (int r=0; r<=data()->rows(); r++) {
    p->drawLine(bbox.left(), y, bbox.right(), y);
    y += data()->rowHeight(r);
    y += vpadding;
  }
  double x = hpadding/2;
  for (int c=0; c<data()->columns(); c++) {
    p->drawLine(x, bbox.top(), x, bbox.bottom());
    x += data()->columnWidth(c);
    x += hpadding;
  }

  c = style().color("canvas-grid-major-color");
  c.setAlphaF(style().real("canvas-grid-major-alpha"));

  p->setPen(QPen(c,
		 style().real("canvas-grid-major-line-width"),
		 Qt::SolidLine,
		 Qt::FlatCap));
  p->drawRect(bbox);
}

QRectF TableBlockItem::boundingRect() const {
  double vpadding = style()->real("table-cell-vpadding");
  double hpadding = style()->real("table-cell-hpadding");
  double y = vpadding;
  double x = hpadding;
  for (int r=0; r<data()->rows(); r++) {
    y += data()->rowHeight(r);
    y += vpadding;
  }
  for (int c=0; c<data()->columns(); c++) {
    x += data()->columnWidth(c);
    x += hpadding;
  }
  return QRectF(0, 0, x, y);
}
