// TableBlockItem.H

#ifndef TABLEBLOCKITEM_H

#define TABLEBLOCKITEM_H

#include "TextBlockItem.H"
#include "TableBlockData.H"

class TTICreator: public TICreator {
public:
  virtual class TextItem *create(TextData *data, Item *parent) const;
};

class TableBlockItem: public TextBlockItem {
  Q_OBJECT;
public:
  TableBlockItem(TableBlockData *data, Item *parent=0);
  virtual ~TableBlockItem();
  DATAACCESS(TableBlockData);
  class TableItem *table();
private:
  class TableItem *item_;
};

#endif
