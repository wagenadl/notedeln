// TableBlockData.H

#ifndef TABLEBLOCKDATA_H

#define TABLEBLOCKDATA_H

#include "BlockData.H"

class TableBlockData: public BlockData {
  Q_OBJECT;
  Q_PROPERTY(int nrows READ rows WRITE setRows);
  Q_PROPERTY(int ncols READ columns WRITE setColumns);
public:
  TableBlockData(Data *parent);
  virtual ~TableBlockData();
  int rows() const;
  int columns() const;
  QList<double> const &rowHeights() const;
  QList<double> const &columnWidths() const;
  double rowHeight(int r) const;
  double columnWidth(int c) const;
  void setRows(int R);
  void setColumns(int C);
  void setRowHeight(int r, double h);
  void setColumnWidth(int c, double w);
  //
  void insertRowBefore(int r);
  void addRow();
  void deleteRow(int r);
  void insertColumnBefore(int c);
  void addColumn();
  void deleteColumn(int c);
  //
  class TableRowData *row(int r);
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
private:
  void fixRowIndices();
private:
  int nrows;
  int ncols;
  QList<double> rh;
  QList<double> cw;
  
};

#endif
