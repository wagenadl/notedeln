// TableCellItem.H

#ifndef TABLECELLITEM_H

#define TABLECELLITEM_H

#include "TextItem.H"
#include "TableCellData.H"

class TableCellItem: public TextItem {
  Q_OBJECT;
public:
  TableCellItem(TableCellData *data, Item *parent=0);
  virtual ~TableCellItem();
  DATAACCESS(TableCellData);
  double fittedHeight() const;
  double fittedWidth() const;
};

#endif
