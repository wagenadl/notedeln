// Items/TableItem.H - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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
