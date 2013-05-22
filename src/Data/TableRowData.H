// TableRowData.H

#ifndef TABLEROWDATA_H

#define TABLEROWDATA_H

#include "Data.H"

class TableRowData: public Data {
  Q_OBJECT;
  Q_PROPERTY(int ncells READ cells WRITE setCells);
public:
  TableRowData(Data *parent);
  virtual ~TableRowData();
  int cells() const;
  void setCells(int C);
  //
  void insertCellBefore(int c);
  void addCell();
  void deleteCell(int c);
  //
  class TableCellData *cell(int c);
  class TableBlockData *table() const;
  //
  void setRowIndex(int r); // for TableBlockData only!
  int rowIndex() const;
  double height() const;
  void setHeight(double h);
private:
  void fixCellIndices();
private:
  int ncells;
  int ridx;
};

#endif
