// TableColumn.H

#ifndef TABLECOLUMN_H

#define TABLECOLUMN_H

class TableColumn: public QObject {
  Q_OBJECT;
public:
  TableColumn(TableBlockItem *parent, int c);
  virtual ~TableColumn();
  class TableCellItem *cell(int r);
  void setPos(double x);
signals:
  void widthChanged();
public slots:
  void reassessWidth(int r);
  void regrabCells(int c);
private:
  class TableBlockData *tableData();
  class TableBlockItem *tableItem();
private:
  class QSignalMapper *ccChangeMapper;
  int cidx;
  QList<class TableCellItem *> cells; // we do not own, of course
  int rformaxw;
};

#endif
