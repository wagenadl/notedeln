// Items/TableItem.H - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// TableItem.H

#ifndef TABLEITEM_H

#define TABLEITEM_H

#include "TextItem.h"
#include "TableData.h"
#include "TextCursor.h"
#include <QList>
#include "TableCell.h"

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
  void deleteRows(int r0, int nr);
  void deleteColumns(int c0, int nc);
  QList<TextCursor> normalizeSelection() const;
  bool isWholeCellSelected() const;
  bool selectionSpansCells() const;
  bool nothingAfterCursor() const;
  bool pasteMultiCell(QString txt); // true if successful
public:
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
public slots:
  bool normalizeCursorPosition(); // true if changed
protected slots:
  virtual void docChange();
signals:
  void unicellular(TableData *); // emitted when we are left as just one cell
signals: 
  void mustNormalizeCursor(); // private use only: must enforce cursor posn
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void focusInEvent(QFocusEvent *);
  virtual bool tryToPaste(bool nonewlines=true);
  virtual void representCursor(QList<TransientMarkup> &) const;
private:
  bool keyPressAsMotion(QKeyEvent *e);
  bool keyPressWithControl(QKeyEvent *e);
  TableCell cellAtCursor() const;
  TableCellRange selectedCells() const;
  TextCursor cursorSelectingCell(TableCell const &cel) const;
  void deleteSelection();
  void tryToCopyCells(class TableCellRange const &) const;
  void joinColumnWithNext(int col); // only for single-row tables
  void splitColumn(int col, int offset); // only for single-row tables
private:
  bool ignoreChanges;
  int ctrla_r0, ctrla_c0;
};

#endif
