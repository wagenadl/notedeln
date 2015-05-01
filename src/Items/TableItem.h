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

#include "TextItem.h"
#include "TableData.h"
#include "TextCursor.h"
#include <QList>

class TableItem: public TextItem {
  Q_OBJECT;
public:
  class Cell {
  public:
    Cell(TableItem const *tbl=NULL, int r=-1, int c=-1):
      tbl(tbl), r(r), c(c) { }
    int row() const { return r; }
    int column() const { return c; }
    void setRow(int r1) { r = r1; }
    void setColumn(int c1) { c = c1; }
    bool isValid() const;
    int firstPosition() const;
    int lastPosition() const;
    bool isEmpty() const;
    bool operator==(Cell const &a) const;
    bool operator!=(Cell const &a) const { return !operator==(a); }
  private:
    TableItem const *tbl;
    int r, c;
  };
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
  bool isCellEmpty(int r, int c) const;
  bool isColumnEmpty(int c) const;
  bool isRowEmpty(int r) const;
  int lastNonEmptyCellInRow(int r) const; // 0 if none
  QList<TextCursor> normalizeSelection(TextCursor const &) const;
  bool isWholeCellSelected(TextCursor const &) const;
  bool selectionSpansCells(TextCursor const &) const;
  bool nothingAfter(TextCursor const &) const;
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
private:
  bool keyPressAsMotion(QKeyEvent *e);
  bool keyPressWithControl(QKeyEvent *e);
  Cell firstCellAt(TextCursor const &) const;
  Cell lastCellAt(TextCursor const &) const;
  Cell cellAt(int pos) const;
  Cell cell(int c, int r) const;
  TextCursor cursorRestrictedTo(Cell const &) const;
private:
  bool ignoreChanges;
  int ctrla_r0, ctrla_c0;
};

#endif
