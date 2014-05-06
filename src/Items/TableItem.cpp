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

  connect(this, SIGNAL(mustNormalizeCursor()),
	  SLOT(normalizeCursorPosition()),
	  Qt::QueuedConnection);
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
  QTextCursor cursor(textCursor());
  switch (e->key()) {
  case Qt::Key_Backspace:
    if (!cursor.hasSelection()) {
      if (col==0 && isRowEmpty(row) && table->rows()>1) {
	deleteRows(row, 1);
	if (row>0)
	  gotoCell(row-1, lastNonEmptyCellInRow(row));
	else
	  normalizeCursorPosition();
	return true;
      } else if (isColumnEmpty(col) && table->columns()>1) {
	deleteColumns(col, 1);
	if (col>0)
	  gotoCell(row, col-1);
	else
	  normalizeCursorPosition();
	return true;
      }
    }
    break;
  case Qt::Key_Delete:
    if (!cursor.hasSelection()) {
      if (col==0 && isRowEmpty(row) && table->rows()>1) {
	deleteRows(row, 1);
	gotoCell(row, col);
	normalizeCursorPosition();
	return true;
      } else if (isColumnEmpty(col) && table->columns()>1) {
	deleteColumns(col, 1);
	gotoCell(row, col);
	normalizeCursorPosition();
	return true;
      }
    }
    break;
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
      gotoCell(row, col, true);
    } else {
      ++col;
      if (col>=table->columns()) 
	insertColumn(col);
      gotoCell(row, col);
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
    gotoCell(row, col, true);
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
      gotoCell(row-1, 0, true);
    } else {
      if (row>=table->rows()-1)
        insertRow(row+1);
      gotoCell(row+1, 0, true);
    }
    return true;
  default:
    break;
  }
  return false;
}

bool TableItem::tryToPaste() {
  qDebug() << "TableItem::tryToPaste";
  return TextItem::tryToPaste();
}

bool TableItem::keyPressWithControl(QKeyEvent *e) {
  if (!(e->modifiers() & Qt::ControlModifier))
    return false;
  QTextCursor cursor(textCursor());
  bool selcel = false;
  int r0, nr, c0, nc;  
  if (cursor.hasComplexSelection()) {
    cursor.selectedTableCells(&r0, &nr, &c0, &nc);
    selcel = true;
  } else {
    QTextTableCell c(table->cellAt(cursor));
    r0 = c.row();
    c0 = c.column();
    nr = 1;
    nc = 1;
    selcel = isWholeCellSelected(cursor);
  }
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
      if (nr==table->rows() && nc==table->columns()) {
	// everything selected; cycle back to just one cell
	selectCell(ctrla_r0, ctrla_c0);
      } else if (nr==table->rows()) {
	// column selected -> select table
	QTextCursor cur0(table->cellAt(0,0).firstCursorPosition());
	QTextCursor cur1(table->cellAt(table->rows()-1, table->columns()-1)
			 .lastCursorPosition());
	cur0.setPosition(cur1.position(), QTextCursor::KeepAnchor);
	setTextCursor(cur0);
      } else if (nc==table->columns()) {
	// row selected -> select column
	if (ctrla_c0<0)
	  ctrla_c0 = 0;
	else if (ctrla_c0>=table->columns())
	  ctrla_c0 = table->columns()-1;
	QTextCursor cur0(table->cellAt(0,ctrla_c0).firstCursorPosition());
	QTextCursor cur1(table->cellAt(table->rows()-1,ctrla_c0)
			 .lastCursorPosition());
	cur0.setPosition(cur1.position(), QTextCursor::KeepAnchor);
	setTextCursor(cur0);
      } else {
	// less than a row, less than a column -> select row
	  if (ctrla_r0<r0)
	    ctrla_r0 = r0;
	  else if (ctrla_r0>=r0+nr)
	    ctrla_r0 = r0+nr-1;
	  QTextCursor cur0(table->cellAt(ctrla_r0,0).firstCursorPosition());
	  QTextCursor cur1(table->cellAt(ctrla_r0, table->columns()-1)
			   .lastCursorPosition());
	  cur0.setPosition(cur1.position(), QTextCursor::KeepAnchor);
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
  case Qt::Key_8: case Qt::Key_Asterisk:
    foreach (QTextCursor c, normalizeSelection(textCursor()))
      toggleSimpleStyle(MarkupData::Bold, c);
    return true;
  case Qt::Key_Underscore:
    foreach (QTextCursor c, normalizeSelection(textCursor()))
      toggleSimpleStyle(MarkupData::Underline, c);
    return true;
  case Qt::Key_1: case Qt::Key_Exclam:
    foreach (QTextCursor c, normalizeSelection(textCursor()))
      toggleSimpleStyle(MarkupData::Emphasize, c);
    return true;
  case Qt::Key_Equal:
    foreach (QTextCursor c, normalizeSelection(textCursor()))
      toggleSimpleStyle(MarkupData::StrikeThrough, c);
    return true;
  default:
    return false;
  }
  return false;
}

bool TableItem::normalizeCursorPosition() {
  QTextCursor cursor(textCursor());
  if (table->cellAt(cursor).isValid()) {
    qDebug() << "normalize position: valid";
    return false;
  }

  if (cursor.atStart()) {
    QTextCursor p0 = table->cellAt(0,0).firstCursorPosition();
    qDebug() << "normalize: at start" << p0.position();
    setTextCursor(p0);
  } else { // assume at end
    QTextCursor p0(table->cellAt(data()->rows()-1, data()->columns()-1)
		   .lastCursorPosition());
    qDebug() << "normalize: at end" << p0.position();
    setTextCursor(p0);
  }
  return true;
}

bool TableItem::mousePress(QGraphicsSceneMouseEvent *e) {
  emit mustNormalizeCursor();
  return TextItem::mousePress(e);
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
    else if (TextItem::keyPress(e)) {
      if (!table->cellAt(textCursor()).isValid()) {
	setTextCursor(cursor); // revert cursor before trying to move
	emit futileMovementKey(e->key(), e->modifiers());
      }
      return true;
    } else
      return false;
  } else if (cursor.atStart()) {
    // before table - this should not be happening any more
    emit futileMovementKey(key, mod);
    return true;
  } else if (cursor.atEnd()) {
    // after table - this should not be happening any more
    emit futileMovementKey(key, mod);
    return true;
  } else {
    // don't know where we are. no good.
    emit futileMovementKey(key, mod);
    return true;
  }
}

bool TableItem::isCellEmpty(int r, int c) const {
  if (r<0 || c<0 || r>=table->rows() || c>=table->columns())
    return false;
  QTextTableCell cell(table->cellAt(r, c));
  if (!cell.isValid())
    return false;
  return cell.lastCursorPosition().position()
    == cell.firstCursorPosition().position();
}

bool TableItem::isColumnEmpty(int c) const {
  for (int r=0; r<table->rows(); r++)
    if (!isCellEmpty(r, c))
      return false;
  return true;
}
  
bool TableItem::isRowEmpty(int r) const {
  for (int c=0; c<table->columns(); c++)
    if (!isCellEmpty(r, c))
      return false;
  return true;
}

int TableItem::lastNonEmptyCellInRow(int r) const {
  for (int c=table->columns()-1; c>0; c--)
    if (!isCellEmpty(r, c))
      return c;
  return 0;
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
  if (r0+n>rows)
    n = rows - r0;
  if (r0==0 && n==rows)
    n--; // refuse to delete all rows
  if (n<=0)
    return;
  
  data()->setRows(rows - n);
  table->removeRows(r0, n);

  if (data()->rows()==1 && data()->columns()==1)
    emit unicellular(data());
}

void TableItem::deleteColumns(int c0, int n) {
  int cols = data()->columns();
  if (c0<0) {
    n += c0;
    c0 = 0;
  }
  if (c0+n>cols)
    n = cols - c0;
  if (c0==0 && n==cols)
    n--; // refuse to delete all columns
  if (n<=0)
    return;
  
  data()->setColumns(cols - n);
  table->removeColumns(c0, n);

  if (data()->rows()==1 && data()->columns()==1)
    emit unicellular(data());
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

  for (int r=r0; r<=r1; r++) {
    for (int c=c0; c<=c1; c++) {
      QTextCursor m=cursor;
      m.setPosition(table->cellAt(r,c).firstCursorPosition().position());
      m.setPosition(table->cellAt(r,c).lastCursorPosition().position(),
                    QTextCursor::KeepAnchor);
      lst << m;
    }
  }

  return lst;
}

bool TableItem::isWholeCellSelected(QTextCursor const &cursor) const {
  QTextTableCell c0 = table->cellAt(cursor.selectionStart());
  QTextTableCell c1 = table->cellAt(cursor.selectionEnd());
  if (!c0.isValid() || !c1.isValid())
    return false;
  return cursor.selectionStart() == c0.firstCursorPosition().position()
    &&  cursor.selectionEnd() == c1.lastCursorPosition().position();
}

bool TableItem::focusIn(QFocusEvent *e) {
  emit mustNormalizeCursor();
  return TextItem::focusIn(e);
}
