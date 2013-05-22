// TableItem.H

#ifndef TABLEITEM_H

#define TABLEITEM_H

#include "TextItem.H"
#include "TableData.H"

class TableItem: public TextItem {
  Q_OBJECT;
public:
  TableItem(TableData *data, Item *parent);
  virtual ~TableItem();
  DATAACCESS(TableData);
  void gotoCell(int row, int col, bool goEnd=false);
  void selectCell(int row, int col);
  void insertRow(int before);
  void insertColumn(int before);
protected slots:
  virtual void docChange();
protected:
  virtual bool keyPress(QKeyEvent *);
private:
  QTextTableFormat format();
private:
  QTextTable *table;
  bool ignoreChanges;
};

#endif
