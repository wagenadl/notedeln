// TableBlockItem.H

#ifndef TABLEBLOCKITEM_H

#define TABLEBLOCKITEM_H

#include "BlockItem.H"
#include "TableBlockData.H"

class TableBlockItem: public BlockItem {
  Q_OBJECT;
public:
  TableBlockItem(TableBlockData *data, Item *parent=0);
  virtual ~TableBlockItem();
  DATAACCESS(TableBlockData);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  QRectF boundingRect() const;
  // The following update the Data
  void insertRowBefore(int r);
  void addRow();
  void deleteRow(int r);
  void insertColumnBefore(int c);
  void addColumn();
  void deleteColumn(int c);
  //
  int rowIndex(class TableRowItem const *, bool tolerateFailure=false);
  // -1 if not found and tolerateFailure=true
  int rowIndex(class TableCellItem const *, bool tolerateFailure=false);
  class TableRowItem *row(int r);
  class TableColumn *column(int c);
  class TableCellItem *cell(int r, int c);
public slots:
  void sizeToFit();
  void restackRows();
  void restackColumns();
private:
  void makeRows();
  void makeColumns();
private:
  QList<TableColumn *> cols;
};

#endif
