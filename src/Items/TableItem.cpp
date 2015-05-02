// Items/TableItem.cpp - This file is part of eln

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

// TableItem.cpp

#include "TableItem.h"
#include <QTextTable>
#include <QKeyEvent>
#include <QDebug>

TableItem::TableItem(TableData *data, Item *parent):
  TextItem(data, parent) {
  connect(this, SIGNAL(mustNormalizeCursor()),
	  SLOT(normalizeCursorPosition()),
	  Qt::QueuedConnection);
}

TableItem::~TableItem() {
}

void TableItem::docChange() {
  qDebug () << "TableItem::docChange";
  TextItem::docChange();
}

bool TableItem::keyPressAsMotion(QKeyEvent *e) {
  bool shft = e->modifiers() & Qt::ShiftModifier;
  bool ctrl = e->modifiers() & Qt::ControlModifier;
  Cell cel = cellAtCursor();
  int row = cel.row();
  int col = cel.column();
  switch (e->key()) {
  case Qt::Key_Backspace:
    if (cursor.hasSelection()) {
      qDebug() << "TableItem: delete selection";
    } else if (col==0 && isRowEmpty(row) && data()->rows()>1) {
      deleteRows(row, 1);
      if (row>0)
	gotoCell(row-1, lastNonEmptyCellInRow(row), true);
      else
	normalizeCursorPosition();
    } else if (isColumnEmpty(col) && data()->columns()>1) {
      deleteColumns(col, 1);
      if (col>0)
	gotoCell(row, col-1, true);
      else
	normalizeCursorPosition();
    } else {
      if (cursor.position() > cel.firstPosition()) 
	cursor.deletePreviousChar();
      else if (col>0)
	gotoCell(row, col-1, true);
      else if (row>0)
	gotoCell(row-1, data()->columns()-1, true);
    }
    return true;
  case Qt::Key_Delete:
    if (cursor.hasSelection()) {
      qDebug() << "TableItem: delete selection";
    } else if (col==0 && isRowEmpty(row) && data()->rows()>1) {
      deleteRows(row, 1);
      gotoCell(row, col);
      normalizeCursorPosition();
    } else if (isColumnEmpty(col) && data()->columns()>1) {
      deleteColumns(col, 1);
      gotoCell(row, col);
      normalizeCursorPosition();
    } else if (cursor.position() < cel.lastPosition()) {
      cursor.deleteChar();
    }
    return true;
  case Qt::Key_Tab:
    if (shft && ctrl) 
      insertColumn(col++);
    else if (ctrl)
      insertColumn(col+1);
    if (shft) {
      --col;
      if (col<0) {
        --row;
        col = data()->columns()-1;
      }
      gotoCell(row, col, true);
    } else {
      ++col;
      if (col>=int(data()->columns()))
	insertColumn(col);
      gotoCell(row, col);
    }
    return true;
  case Qt::Key_Backtab:
    if (ctrl) 
      insertColumn(col++);
    --col;
    if (col<0) {
      --row;
      col = data()->columns()-1;
    }
    gotoCell(row, col, true);
    return true;
  case Qt::Key_Enter: case Qt::Key_Return:
    if (shft && ctrl)
      insertRow(row++);
    else if (ctrl)
      insertRow(row+1);
    if (shft) {
      if (row==0)
        insertRow(row++);
      gotoCell(row-1, 0, true);
    } else {
      if (nothingAfterCursor()) {
	cursor.movePosition(TextCursor::End);
	emit futileMovementKey(e->key(), e->modifiers());
      } else {
	if (row>=data()->rows()-1)
	  insertRow(row+1);
	gotoCell(row+1, 0, true);
      }
    }
    return true;
  default:
    break;
  }
  return false;
}

bool TableItem::nothingAfterCursor() const {
  Cell cel(cellAtCursor());
  if (cel.row()<data()->rows()-1)
    return false;
  if (cursor.position()!=cel.lastPosition())
    return false;
  for (int c=cel.column()+1; c<data()->columns(); c++) 
    if (!cell(cel.row(), c).isEmpty())
      return false;
  return true;
}

bool TableItem::tryToPaste(bool /*noparagraphs*/) {
  return TextItem::tryToPaste(true);
}

bool TableItem::keyPressWithControl(QKeyEvent *e) {
  if (!(e->modifiers() & Qt::ControlModifier))
    return false;
  CellRange rng = selectedCells();
  bool selcel = isWholeCellSelected();
  int r0 = rng.firstRow();
  int c0 = rng.firstColumn();
  int nr = rng.rows();
  int nc = rng.columns();

  switch (e->key()) {
  case Qt::Key_Delete: case Qt::Key_Backspace: 
    if (selcel) {
      if (nr==int(data()->rows()) && data()->columns()>1) {
        deleteColumns(c0, nc);
	normalizeCursorPosition();
        return true;
      } else if (nc==int(data()->columns()) && data()->rows()>1) {
        deleteRows(r0, nr);
	normalizeCursorPosition();
        return true;
      }
    }
    break;
  case Qt::Key_A: 
    if (selcel) {
      if (nr==int(data()->rows()) && nc==int(data()->columns())) {
	// everything selected; cycle back to just one cell
	selectCell(ctrla_r0, ctrla_c0);
      } else if (nr==int(data()->rows())) {
	// column selected -> select table
	TextCursor cur0(document(), cell(0,0).firstPosition());
	TextCursor cur1(document(), cell(data()->columns()-1,
                                         data()->rows()-1).lastPosition());
	cur0.setPosition(cur1.position(), TextCursor::KeepAnchor);
	setTextCursor(cur0);
      } else if (nc==int(data()->columns())) {
	// row selected -> select column
	if (ctrla_c0<0)
	  ctrla_c0 = 0;
	else if (ctrla_c0>=int(data()->columns()))
	  ctrla_c0 = int(data()->columns()-1);
	TextCursor cur0(document(), cell(ctrla_c0, 0).firstPosition());
	TextCursor cur1(document(),
                        cell(ctrla_c0, data()->rows()-1).lastPosition());
	cur0.setPosition(cur1.position(), TextCursor::KeepAnchor);
	setTextCursor(cur0);
      } else {
	// less than a row, less than a column -> select row
	  if (ctrla_r0<r0)
	    ctrla_r0 = r0;
	  else if (ctrla_r0>=r0+nr)
	    ctrla_r0 = r0+nr-1;
	  TextCursor cur0(document(), cell(0, ctrla_r0).firstPosition());
	  TextCursor cur1(document(), cell(data()->columns()-1, ctrla_r0)
                          .lastPosition());
	  cur0.setPosition(cur1.position(), TextCursor::KeepAnchor);
	  setTextCursor(cur0);
      }
    } else {
      // no cells selected -> select cell
      ctrla_r0 = r0; ctrla_c0 = c0;
      selectCell(r0, c0);
    }
    return true;    
  case Qt::Key_V:
    tryToPaste();
    return true;
  case Qt::Key_N:
    if (nr==1 && nc==1)
      tryFootnote(); // footnote refs cannot span cells
    return true; 
  case Qt::Key_L:
    if (nr==1 && nc==1)
      tryExplicitLink(); // links cannot span cells
    return true;
  default:
    foreach (TextCursor c, normalizeSelection())
      if (!keyPressAsSimpleStyle(e->key(), c))
	return false;
    return true;
  }
  return false;
}

bool TableItem::normalizeCursorPosition() {
  TextCursor c0 = cursor;
  cursor.clampPosition();
  if (c0!=cursor) {
    qDebug() << "TableItem::normalizeCursorPosition actually did something";
    return true;
  } else {
    return false;
  }
}

void TableItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  emit mustNormalizeCursor();
  TextItem::mousePressEvent(e);
}

void TableItem::keyPressEvent(QKeyEvent *e) {
  if (keyPressAsMotion(e)) {
    ;
  } else if (keyPressWithControl(e)) {
    ;
  } else if (!selectionSpansCells()) {
    TextItem::keyPressEvent(e);
  }
}

bool TableItem::isCellEmpty(int r, int c) const {
  if (r<0 || c<0 || r>=data()->rows() || c>=data()->columns())
    return false;
  Cell cell(this, r, c);
  if (!cell.isValid())
    return false;
  return cell.firstPosition()==cell.lastPosition();
}

bool TableItem::isColumnEmpty(int c) const {
  for (int r=0; r<data()->rows(); r++)
    if (!isCellEmpty(r, c))
      return false;
  return true;
}
  
bool TableItem::isRowEmpty(int r) const {
  for (int c=0; c<data()->columns(); c++)
    if (!isCellEmpty(r, c))
      return false;
  return true;
}

int TableItem::lastNonEmptyCellInRow(int r) const {
  for (int c=data()->columns()-1; c>0; c--)
    if (!isCellEmpty(r, c))
      return c;
  return 0;
}

void TableItem::gotoCell(int r, int c, bool toEnd) {
  int C = data()->columns();
  int R = data()->rows();
  if (r<0)
    r = 0;
  if (c<0)
    c = 0;
  if (r>=R)
    r = R - 1;
  if (c>=C)
    c = C - 1;
  ASSERT(r>=0 && c>=0);
  if (toEnd)
    setTextCursor(TextCursor(document(), cell(r,c).lastPosition()));
  else
    setTextCursor(TextCursor(document(), cell(r,c).firstPosition()));
}

void TableItem::selectCell(int r, int c) {
  gotoCell(r, c);
  cursor.setPosition(cell(r, c).lastPosition(),
		     TextCursor::KeepAnchor);
  update();
}

void TableItem::deleteRows(int r0, int n) {
  int C = data()->columns();
  int R = data()->rows();
  if (r0<0) {
    n += r0;
    r0 = 0;
  }
  if (r0+n>R)
    n = R - r0;
  if (r0==0 && n==R)
    n--; // refuse to delete all rows
  if (n<=0)
    return;

  QStringList oldcells = data()->text().split("\n");
  // first and last are always empty, then come the actual cells
  QStringList newcells;

  newcells << oldcells.takeFirst();
  for (int r=0; r<r0; r++) 
    for (int c=0; c<C; c++)
      newcells << oldcells.takeFirst();
  for (int r=r0; r<r0+n; r++)
    for (int c=0; c<C; c++)
      oldcells.takeFirst();
  for (int r=r0+n; r<R; r++)
    for (int c=0; c<C; c++)
      newcells << oldcells.takeFirst();
  newcells << oldcells.takeFirst();
  ASSERT(oldcells.isEmpty());

  data()->setRows(R - n);
  data()->setText(newcells.join("\n"));
  text->relayout();
  docChange();

  if (R-n==1 && C==1)
    emit unicellular(data());
}

void TableItem::deleteColumns(int c0, int n) {
  int R = data()->rows();
  int C = data()->columns();
  if (c0<0) {
    n += c0;
    c0 = 0;
  }
  if (c0+n>C)
    n = C - c0;
  if (c0==0 && n==C)
    n--; // refuse to delete all columns
  if (n<=0)
    return;

  QStringList oldcells = data()->text().split("\n");
  // first and last are always empty, then come the actual cells
  QStringList newcells;
  newcells << oldcells.takeFirst();
  for (int r=0; r<R; r++) {
    for (int c=0; c<c0; c++)
      newcells << oldcells.takeFirst();
    for (int c=c0; c<c0+n; c++)
      oldcells.takeFirst();
    for (int c=c0+n; c<C; c++)
      newcells << oldcells.takeFirst();
  }
  newcells << oldcells.takeFirst();
  ASSERT(oldcells.isEmpty());
  
  data()->setColumns(C - n);
  data()->setText(newcells.join("\n"));
  text->relayout();
  docChange();
  if (C-n==1 && R==1)
    emit unicellular(data());
}

void TableItem::insertRow(int before) {
  int C = data()->columns();
  int R = data()->rows();
  if (before<0)
    before = 0;
  if (before>R)
    before = R;

  QStringList oldcells = data()->text().split("\n");
  // first and last are always empty, then come the actual cells
  QStringList newcells;

  newcells << oldcells.takeFirst();
  for (int r=0; r<before; r++) 
    for (int c=0; c<C; c++)
      newcells << oldcells.takeFirst();
  for (int c=0; c<C; c++)
    newcells << "";
  for (int r=before; r<R; r++)
    for (int c=0; c<C; c++)
      newcells << oldcells.takeFirst();
  newcells << oldcells.takeFirst();
  ASSERT(oldcells.isEmpty());
  
  data()->setRows(R+1);
  data()->setText(newcells.join("\n"));
  text->relayout();
  docChange();  
}

void TableItem::insertColumn(int before) {
  int R = data()->rows();
  int C = data()->columns();

  if (before<0)
    before = 0;
  if (before>C)
    before = C;

  QStringList oldcells = data()->text().split("\n");
  // first and last are always empty, then come the actual cells
  QStringList newcells;

  newcells << oldcells.takeFirst();
  for (int r=0; r<R; r++) {
    for (int c=0; c<before; c++)
      newcells << oldcells.takeFirst();
    newcells << "";
    for (int c=before; c<C; c++)
      newcells << oldcells.takeFirst();
  }
  newcells << oldcells.takeFirst();
  ASSERT(oldcells.isEmpty());
  
  data()->setColumns(C+1);
  data()->setText(newcells.join("\n"));
  text->relayout();
  docChange();  
}

QList<TextCursor> TableItem::normalizeSelection() const {
  QList<TextCursor> lst;
  if (selectionSpansCells()) {
    CellRange rng = selectedCells();
    for (int r=rng.firstRow(); r<=rng.lastRow(); r++) 
      for (int c=rng.firstColumn(); c<=rng.lastColumn(); c++) 
        lst << cursorSelectingCell(cell(r, c));
  } else {
    lst << cursor;
  }
  return lst;
}

TextCursor TableItem::cursorSelectingCell(Cell const &cel) const {
  TextCursor curs = cursor;
  curs.setPosition(cel.firstPosition());
  curs.setPosition(cel.lastPosition(), TextCursor::KeepAnchor);
  return curs;
}

bool TableItem::selectionSpansCells() const {
  CellRange cr = selectedCells();
  return cr.columns()>1 || cr.rows()>1;
}
  

bool TableItem::isWholeCellSelected() const {
  if (!cursor.hasSelection())
    return false;
  if (selectionSpansCells())
    return true;
  Cell cel = cellAtCursor();
  TextCursor::Range r = cursor.selectedRange();
  return r.start()==cel.firstPosition() && r.end()==cel.lastPosition();
}

void TableItem::focusInEvent(QFocusEvent *e) {
  emit mustNormalizeCursor();
  TextItem::focusInEvent(e);
}

bool TableItem::Cell::isValid() const {
  if (tbl && c>=0 && r>=0
      && c<tbl->data()->columns()
      && r<tbl->data()->rows())
    return true;
  qDebug() << "TableItem::Cell::notValid" << tbl << r << c
           << "(" << tbl->data()->rows() << tbl->data()->columns() << ")";
  return false;
}

bool TableItem::Cell::operator==(Cell const &a) const {
  return tbl==a.tbl && c==a.c && r==a.r;
}

int TableItem::Cell::firstPosition() const {
  ASSERT(isValid());
  return tbl->data()->cellStart(r, c);
}

int TableItem::Cell::lastPosition() const {
  ASSERT(isValid());
  return tbl->data()->cellStart(r, c) + tbl->data()->cellLength(r, c);
}

bool TableItem::Cell::isEmpty() const {
  return firstPosition()==lastPosition();
}

TableItem::Cell TableItem::cell(int row, int col) const {
  return Cell(this, row, col);
}

TableItem::CellRange TableItem::selectedCells() const {
  TextCursor::Range rr = cursor.selectedRange();
  Cell a = cellAt(rr.start());
  Cell b = cellAt(rr.end());
  int r0 = a.row();
  int dr = b.row() - a.row();
  if (dr<0) {
    r0 += dr;
    dr = -dr;
  }
  int c0 = a.column();
  int dc = b.column() - a.column();
  if (dc<0) {
    c0 += dc;
    dc = -dc;
  }
  return CellRange(this, r0, c0, dr+1, dc+1);
}

TableItem::Cell TableItem::cellAtCursor() const {
  return cellAt(cursor.position());
}

TableItem::Cell TableItem::cellAt(int pos) const {
  int C = data()->columns();
  int R = data()->rows();
  for (int r=0; r<R; r++) {
    for (int c=0; c<C; c++) {
      Cell cel(this, r, c);
      if (pos>=cel.firstPosition() && pos<=cel.lastPosition())
        return cel;
    }
  }
  return Cell();
}


QList<TransientMarkup> TableItem::representCursor() const {
  QList<TransientMarkup> tmm;
  if (cursor.hasSelection()) {
    if (selectionSpansCells()) {
      CellRange rng = selectedCells();
      for (int r=rng.firstRow(); r<=rng.lastRow(); r++)
        for (int c=rng.firstColumn(); c<=rng.lastColumn(); c++)
          tmm << TransientMarkup(cell(r,c).firstPosition(),
                                 cell(r,c).lastPosition(),
                                 MarkupData::Selected);
    } else {
      tmm << TransientMarkup(cursor.selectionStart(), cursor.selectionEnd(),
                             MarkupData::Selected);
    }
  }
  return tmm;
}

