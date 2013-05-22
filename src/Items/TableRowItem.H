// TableRowItem.H

#ifndef TABLEROWITEM_H

#define TABLEROWITEM_H

#include "Item.H"
#include "TableRowData.H"

class TableRowItem: public Item {
  Q_OBJECT;
public:
  TableRowItem(TableRowData *data, Item *parent=0);
  virtual ~TableRowItem();
  DATAACCESS(TableRowData);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  QRectF boundingRect() const;
  // The following update the Data
  void insertCellBefore(int c);
  void addCell();
  void deleteCell(int c);
  //
  int columnIndex(class TableCellItem const *, bool tolerateFailure=false);
  class TableCellItem *cell(int c);
public slots:
  void sizeToFit();
signals:
  void heightChanged();
private:
  void makeCells();
  void remap();
private slots:
  void reassessHeight(int c);
private:
  class QSignalMapper *ccChangeMapper;
  int cformaxh;
  QList<TableCellItem*> cells;
};

#endif
