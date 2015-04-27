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
  TextItem(data, parent, true) {
  /* The plain text has been written into the QTextDocument, but the
     table has not been constructed yet, so unfortunately, we'll have to
     rebuild the document.
  */
  qDebug() << "TableItems not currently functional";
  /*
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
  */
}

TableItem::~TableItem() {
}

/*
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
*/

void TableItem::docChange() {
  /*
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
  */
  TextItem::docChange();
}

bool TableItem::keyPressAsMotion(QKeyEvent *e, TableItem::Cell const &cell) {
  bool shft = e->modifiers() & Qt::ShiftModifier;
  bool ctrl = e->modifiers() & Qt::ControlModifier;
  int row = cell.row();
  int col = cell.column();
  TextCursor cursor(textCursor());
  switch (e->key()) {
  case Qt::Key_Backspace:
    if (!cursor.hasSelection()) {
      if (col==0 && isRowEmpty(row) && data()->rows()>1) {
	deleteRows(row, 1);
	if (row>0)
	  gotoCell(row-1, lastNonEmptyCellInRow(row), true);
	else
	  normalizeCursorPosition();
	return true;
      } else if (isColumnEmpty(col) && data()->columns()>1) {
	deleteColumns(col, 1);
	if (col>0)
	  gotoCell(row, col-1, true);
	else
	  normalizeCursorPosition();
	return true;
      }
    }
    break;
  case Qt::Key_Delete:
    if (!cursor.hasSelection()) {
      if (col==0 && isRowEmpty(row) && data()->rows()>1) {
	deleteRows(row, 1);
	gotoCell(row, col);
	normalizeCursorPosition();
	return true;
      } else if (isColumnEmpty(col) && data()->columns()>1) {
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
    qDebug() << "backtab";
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
      if (row>=int(data()->rows()-1))
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
  TextCursor cursor(textCursor());
  bool selcel = false;
  int r0, nr, c0, nc;  
  /* if (cursor.hasComplexSelection()) {
    cursor.selectedTableCells(&r0, &nr, &c0, &nc);
    selcel = true;
  } else */ {
    Cell c(cellAt(cursor));
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
    /*
    if (!cursor.hasComplexSelection())
      tryFootnote(); // footnote refs cannot span cells
    */
    return true; 
  case Qt::Key_L:
    /*
    if (!cursor.hasComplexSelection())
      tryExplicitLink(); // links cannot span cells
    */
    return true;
  default:
    foreach (TextCursor c, normalizeSelection(textCursor()))
      if (!keyPressAsSimpleStyle(e->key(), c))
	return false;
    return true;
  }
  return false;
}

bool TableItem::normalizeCursorPosition() {
  TextCursor cursor(textCursor());
  if (cellAt(cursor).isValid()) {
    qDebug() << "normalize position: valid";
    return false;
  }

  if (cursor.atStart()) {
    TextCursor p0(document(), cell(0,0).firstPosition());
    qDebug() << "normalize: at start" << p0.position();
    setTextCursor(p0);
  } else { // assume at end
    TextCursor p0(document(), cell(data()->columns()-1, data()->rows()-1)
                  .lastPosition());
    qDebug() << "normalize: at end" << p0.position();
    setTextCursor(p0);
  }
  return true;
}

void TableItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  emit mustNormalizeCursor();
  TextItem::mousePressEvent(e);
}

void TableItem::keyPressEvent(QKeyEvent *e) {
  /* This is where we implement insertion and deletion of rows and columns,
     augmented tab/enter navigation, and prevention of out-of-table text
     insertion.
     We do not, here, have to worry about routine maintenance of cell sizes;
     that happens in docChange().
  */
  TextCursor cursor(textCursor());
  Cell cell = cellAt(cursor);
  int key = e->key();
  Qt::KeyboardModifiers mod = e->modifiers();
  if (cell.isValid()) {
    // inside the table
    if (keyPressAsMotion(e, cell)) {
      ;
    } else if (keyPressWithControl(e)) {
      ;
    } else {
      TextItem::keyPressEvent(e);
      if (cellAt(textCursor()).isValid()) {
	setTextCursor(cursor); // revert cursor before trying to move
	emit futileMovementKey(e->key(), e->modifiers());
      }
    }
  } else {
    e->ignore();
  }
}

bool TableItem::isCellEmpty(int r, int c) const {
  if (r<0 || c<0 || r>=data()->rows() || c>=data()->columns())
    return false;
  Cell cell(this, c, r);
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
  if (r<0)
    r=0;
  if (c<0)
    c=0;
  if (r>=data()->rows())
    r=data()->rows()-1;
  if (c>=data()->columns())
    c=data()->columns()-1;
  ASSERT(r>=0 && c>=0);
  Cell cel(cell(c, r));
  ASSERT(cel.isValid());
  if (toEnd)
    setTextCursor(TextCursor(document(), cel.lastPosition()));
  else
    setTextCursor(TextCursor(document(), cel.firstPosition()));
}

void TableItem::selectCell(int r, int c) {
  gotoCell(r, c);
  TextCursor cursor(textCursor());
  Cell cel(cellAt(cursor));
  ASSERT(cel.isValid());
  cursor.setPosition(cel.lastPosition(),
		     TextCursor::KeepAnchor);
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
  //table->removeRows(r0, n);
  ASSERT(0);

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
  //table->removeColumns(c0, n);
  ASSERT(0);

  if (data()->rows()==1 && data()->columns()==1)
    emit unicellular(data());
}

void TableItem::insertRow(int before) {
  int rows = data()->rows();
  if (before<0)
    before = 0;
  if (before>rows)
    before = rows;

  data()->setRows(rows+1);

  /*
  if (before==rows)
    table->appendRows(1);
  else
    table->insertRows(before, 1);
  */
  ASSERT(0);
}

void TableItem::insertColumn(int before) {
  int cols = data()->columns();
  if (before<0)
    before = 0;
  if (before>cols)
    before = cols;

  data()->setColumns(cols+1);

  /*
  if (before==cols)
    table->appendColumns(1);
  else
    table->insertColumns(before, 1);
  */
  ASSERT(0);
}

QList<TextCursor> TableItem::normalizeSelection(TextCursor const &cursor)
  const {
  ASSERT(0);
  /*
  QList<TextCursor> lst;
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
  */
}

bool TableItem::isWholeCellSelected(TextCursor const &cursor) const {
  Cell c0 = cellAt(cursor.selectionStart());
  Cell c1 = cellAt(cursor.selectionEnd());
  if (!c0.isValid() || !c1.isValid())
    return false;
  return cursor.selectionStart() == c0.firstPosition()
    &&  cursor.selectionEnd() == c1.lastPosition();
}

void TableItem::focusInEvent(QFocusEvent *e) {
  emit mustNormalizeCursor();
  TextItem::focusInEvent(e);
}

/* Here is an example of a table block in current eln page json:

       { "typ": "tableblock",
      "cre": "2014-03-06T10:35:44",
      "mod": "2014-03-06T10:42:14",
      "h": 298.0,
      "ind": 1,
      "sheet": 0,
      "y0": 176.375,
      "cc": [
        { "typ": "table",
          "cre": "2014-03-06T10:35:44",
          "mod": "2014-03-06T10:42:14",
          "len": [
            13,
            20,
            7,
            25,
            91,
            7,
            14,
            22,
            7,
            13,
            68,
            7,
            10,
            40,
            7,
            12,
            42,
            7
          ],
          "nc": 3,
          "nr": 6,
          "text": "\nXimena Bernal\nAnimal communication\nEEB web\nEsteban Fernandez-Juricic\nEvo of vert. visual system, Antipredator, foraging, and mating behav. Appl. sensory ecology\nEEB web\nRichard Howard\nEvo. of mating systems\nEEB web\nJeffrey Lucas\nAnimal comm., sensory ecol., call cplxty in birds, dynamic game thy.\nEEB web\nKevin Otto\nNeuroprosthesis, sys. n.sci., neurotech.\nD&D web\nDonald Ready\nCell, mol., gen. of Drosophila retina dev.\nD&D web\n"
        }
      ]
    },
*/

/* I am not sure that I really want to keep the LEN field in the
   future. After all, it can be reconstructed from the locations of
   the NL characters.
*/

int TableItem::Cell::firstPosition() const {
  ASSERT(0);
  return 0;
}

int TableItem::Cell::lastPosition() const {
  ASSERT(0);
  return 0;
}

TableItem::Cell TableItem::cell(int col, int row) const {
  return Cell(this, col, row);
}

TableItem::Cell TableItem::cellAt(TextCursor const &tc) const {
  return cellAt(tc.position());
}

TableItem::Cell TableItem::cellAt(int pos) const {
  ASSERT(0);
  return Cell(this, 0, 0);
}
