// TableCellData.H

#ifndef TABLECELLDATA_H

#define TABLECELLDATA_H

#include "TextData.H"

class TableCellData: public TextData {
  Q_OBJECT;
public:
  TableCellData(Data *parent);
  virtual ~TableCellData();
  class TableBlockData *table() const;
  class TableRowData *row() const;
  //
  void setColumnIndex(int r); // for TableRowData only!!!
  int columnIndex() const;
private:
  int cidx;
};

#endif
