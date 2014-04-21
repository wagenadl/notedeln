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

#include "TableItem.H"
#include <QTextDocument>
#include <QTextTable>
#include "TextMarkings.H"
#include <QKeyEvent>
#include <QDebug>

TableItem::TableItem(TableData *data, Item *parent):
  TextItem(data, parent, true) {
  /* The plain text has been written into the QTextDocument, but the
     table has not been constructed yet, so unfortunately, we'll have to
     rebuild the document.
  */
  text->setPlainText("");
  
  QTextCursor c(document());
  table = c.insertTable(data->rows(), data->columns(), format());
  for (unsigned int r=0; r<data->rows(); r++)
    for (unsigned int c=0; c<data->columns(); c++)
      table->cellAt(r, c).firstCursorPosition()
	.insertText(data->cellContents(r, c));

  finalizeConstructor();
}

TableItem::~TableItem() {
}

QTextTableFormat TableItem::format() {
  // should read from style
  QTextTableFormat fmt;
  fmt.setBackground(QColor("#ffffff"));
  fmt.setBorderBrush(QColor("#aaffdd"));
  fmt.setBorderStyle(QTextFrameFormat::BorderStyle_Outset);
  fmt.setCellPadding(4);
  fmt.setCellSpacing(1);
  return fmt;
}

void TableItem::docChange() {
  if (data()->text() == text->toPlainText())
    return; // trivial change

  // Reassess all cells. This could be made a lot smarter.
  // But beware: this method is used by insert row/column.
  for (unsigned int r=0; r<data()->rows(); r++) {
    for (unsigned int c=0; c<data()->columns(); c++) {
      int p0 = table->cellAt(r, c).firstCursorPosition().position();
      int p1 = table->cellAt(r, c).lastCursorPosition().position();
      unsigned int l = p1 - p0;
      if (l != data()->cellLength(r, c))
	data()->setCellLength(r, c, l, true);
    }
  }  
  TextItem::docChange();
}

bool TableItem::keyPressAsMotion(QKeyEvent *e, QTextTableCell const &cell) {
  bool shft = e->modifiers() & Qt::ShiftModifier;
  bool ctrl = e->modifiers() & Qt::ControlModifier;
  int row = cell.row();
  int col = cell.column();
  switch (e->key()) {
  case Qt::Key_Tab:
    qDebug() << "tab";
    if (shft && ctrl) 
      insertColumn(col++);
    else if (ctrl)
      insertColumn(col+1);
    if (shft) {
      --col;
      if (col<0) {
        --row;
        col = table->columns()-1;
      }
      selectCell(row, col);
    } else {
      ++col;
      if (col>=table->columns()) {
        ++row;
        col = 0;
      }
      selectCell(row, col);
    }
    return true;
  case Qt::Key_Backtab:
    qDebug() << "backtab";
    if (ctrl) 
      insertColumn(col++);
    --col;
    if (col<0) {
      --row;
      col = table->columns()-1;
    }
    selectCell(row, col);
    return true;
  case Qt::Key_Enter: case Qt::Key_Return:
    qDebug() << "enter";
    if (shft && ctrl)
      insertRow(row++);
    else if (ctrl)
      insertRow(row+1);
    if (shft) {
      if (row==0)
        insertRow(row++);
      selectCell(row-1, 0);
    } else {
      if (row>=table->rows()-1)
        insertRow(row+1);
      selectCell(row+1, 0);
    }
    return true;
  default:
    return false;
  }
}

bool TableItem::tryToPaste() {
  qDebug() << "TableItem::tryToPaste";
  return TextItem::tryToPaste();
}

bool TableItem::keyPressWithControl(QKeyEvent *e) {
  if (!(e->modifiers() & Qt::ControlModifier))
    return false;
  QTextCursor cursor(textCursor());
  switch (e->key()) {
  case Qt::Key_Delete: case Qt::Key_Backspace:
    if (cursor.hasComplexSelection()) {
      int r0, nr, c0, nc;
      cursor.selectedTableCells(&r0, &nr, &c0, &nc);
      if (nr==int(data()->rows())) {
        deleteColumns(c0, nc);
        return true;
      } else if (nc==int(data()->columns())) {
        deleteRows(r0, r0);
        return true;
      }
    }
    break;
  case Qt::Key_V:
    tryToPaste();
    return true;
  case Qt::Key_N:
    if (!cursor.hasComplexSelection())
      tryFootnote(); // footnote refs cannot span cells
    return true; 
  case Qt::Key_L:
    if (!cursor.hasComplexSelection())
      tryExplicitLink(); // links cannot span cells
    return true;
  case Qt::Key_Slash:
    foreach (QTextCursor c, normalizeSelection(textCursor()))
      toggleSimpleStyle(MarkupData::Italic, c);
    return true;
  case Qt::Key_8:
    foreach (QTextCursor c, normalizeSelection(textCursor()))
      toggleSimpleStyle(MarkupData::Bold, c);
    return true;
  case Qt::Key_Minus:
    foreach (QTextCursor c, normalizeSelection(textCursor()))
      toggleSimpleStyle(MarkupData::Underline, c);
    return true;
  default:
    return false;
  }
  return false;
}

bool TableItem::keyPress(QKeyEvent *e) {
  /* This is where we implement insertion and deletion of rows and columns,
     augmented tab/enter navigation, and prevention of out-of-table text
     insertion.
     We do not, here, have to worry about routine maintenance of cell sizes;
     that happens in docChange().
  */
  QTextCursor cursor(textCursor());
  QTextTableCell cell = table->cellAt(cursor);
  int key = e->key();
  Qt::KeyboardModifiers mod = e->modifiers();
  if (cell.isValid()) {
    // inside the table
    if (keyPressAsMotion(e, cell))
      return true;
    else if (keyPressWithControl(e))
      return true;
    else
      return TextItem::keyPress(e);
  } else if (cursor.atStart()) {
    // before table
    if (key==Qt::Key_Right || key==Qt::Key_Down)
      gotoCell(0, 0);
    else 
      emit futileMovementKey(key, mod);
    return true;
  } else if (cursor.atEnd()) {
    // after table
    if (key==Qt::Key_Left || key==Qt::Key_Up)
      gotoCell(table->rows()-1, table->columns()-1, true);
    else 
      emit futileMovementKey(key, mod);
    return true;
  } else {
    // don't know where we are. no good.
    emit futileMovementKey(key, mod);
    return true;
  }
}

void TableItem::gotoCell(int r, int c, bool toEnd) {
  if (r<0)
    r=0;
  if (c<0)
    c=0;
  if (r>=table->rows())
    r=table->rows()-1;
  if (c>=table->columns())
    c=table->columns()-1;
  ASSERT(r>=0 && c>=0);
  QTextTableCell cell(table->cellAt(r, c));
  ASSERT(cell.isValid());
  if (toEnd)
    setTextCursor(cell.lastCursorPosition());
  else
    setTextCursor(cell.firstCursorPosition());
}

void TableItem::selectCell(int r, int c) {
  gotoCell(r, c);
  QTextCursor cursor(textCursor());
  QTextTableCell cell(table->cellAt(cursor));
  ASSERT(cell.isValid());
  cursor.setPosition(cell.lastCursorPosition().position(),
		     QTextCursor::KeepAnchor);
  setTextCursor(cursor);
}

void TableItem::deleteRows(int r0, int n) {
  int rows = data()->rows();
  if (r0<0) {
    n += r0;
    r0 = 0;
  }
  if (r0+n>=rows)
    n = rows - r0;
  if (r0==0 && n==rows)
    n--; // do not delete last row
  if (n<=0)
    return;
  
  data()->setRows(rows - n);
  table->removeRows(r0, n);
}

void TableItem::deleteColumns(int c0, int n) {
  int cols = data()->columns();
  if (c0<0) {
    n += c0;
    c0 = 0;
  }
  if (c0+n>=cols)
    n = cols - c0;
  if (c0==0 && n==cols)
    n--; // do not delete last column
  if (n<=0)
    return;
  
  data()->setColumns(cols - n);
  table->removeColumns(c0, n);
}

void TableItem::insertRow(int before) {
  int rows = table->rows();
  if (before<0)
    before = 0;
  if (before>rows)
    before = rows;

  data()->setRows(rows+1);

  if (before==rows)
    table->appendRows(1);
  else
    table->insertRows(before, 1);
}

void TableItem::insertColumn(int before) {
  int cols = table->columns();
  if (before<0)
    before = 0;
  if (before>cols)
    before = cols;

  data()->setColumns(cols+1);

  if (before==cols)
    table->appendColumns(1);
  else
    table->insertColumns(before, 1);
}

QList<QTextCursor> TableItem::normalizeSelection(QTextCursor const &cursor)
  const {
  QList<QTextCursor> lst;
  if (!cursor.hasComplexSelection()) {
    lst << cursor;
    return lst;
  }
  int r0, nr, c0, nc;
  cursor.selectedTableCells(&r0, &nr, &c0, &nc);
  int r1 = r0+nr-1;
  int c1 = c0+nc-1;
  qDebug() << "normalizeselection r:" << r0 << r1
           << "c:"<<c0<<c1;

  for (int r=r0; r<=r1; r++) {
    for (int c=c0; c<=c1; c++) {
      QTextCursor m=cursor;
      m.setPosition(table->cellAt(r,c).firstCursorPosition().position());
      m.setPosition(table->cellAt(r,c).lastCursorPosition().position(),
                    QTextCursor::KeepAnchor);
      lst << m;
    }
  }
  qDebug() << "->";
  foreach (QTextCursor x, lst) 
    qDebug() << "  " << x.hasSelection()
             << x.selectionStart() << x.selectionEnd();
  return lst;
}
  
QList<int> TableItem::wholeRowsInSelection(QTextCursor const &cursor) const {
  QList<int> lst;
  if (!cursor.hasComplexSelection())
    return lst;
  int r0, nr, c0, nc;
  cursor.selectedTableCells(&r0, &nr, &c0, &nc);
  if (nc==table->columns()) 
    for (int r=r0; r<r0+nr; r++)
      lst.append(r);
    return lst;
}

QList<int> TableItem::wholeColumnsInSelection(QTextCursor const &cursor) const {
  QList<int> lst;
  if (!cursor.hasComplexSelection())
    return lst;
  int r0, nr, c0, nc;
  cursor.selectedTableCells(&r0, &nr, &c0, &nc);
  if (nr==table->rows()) 
    for (int c=c0; c<c0+nc; c++)
      lst.append(c);
    return lst;
}
