// TableItem.H

#ifndef TABLEITEM_H

#define TABLEITEM_H

#include "TextItem.H"
#include "TableData.H"
#include <QTextCursor>
#include <QList>

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
  QList<QTextCursor> normalizeSelection(QTextCursor const &) const;
  QList<int> wholeRowsInSelection(QTextCursor const &) const;
  QList<int> wholeColumnsInSelection(QTextCursor const &) const;
protected slots:
  virtual void docChange();
protected:
  virtual bool keyPress(QKeyEvent *);
  bool tryToPaste();
private:
  bool keyPressAsMotion(QKeyEvent *, class QTextTableCell const &);
  bool keyPressWithControl(QKeyEvent *);
private:
  QTextTableFormat format();
private:
  QTextTable *table;
  bool ignoreChanges;
};

#endif
