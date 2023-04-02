// Items/TableItem.cpp - This file is part of NotedELN

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

// TableItem.cpp

#include <QPainter>
#include "TableItem.h"
#include <QTextTable>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

TableItem::TableItem(TableData *data, Item *parent):
  TextItem(data, parent) {
  connect(this, &TableItem::mustNormalizeCursor,
	  this, &TableItem::normalizeCursorPosition,
	  Qt::QueuedConnection);
}

TableItem::~TableItem() {
}

void TableItem::docChange() {
  TextItem::docChange();
}

bool TableItem::keyPressAsMotion(QKeyEvent *e) {
  bool shft = e->modifiers() & Qt::ShiftModifier;
  bool ctrl = e->modifiers() & Qt::ControlModifier;
  TableCell cel = cellAtCursor();
  int row = cel.row();
  int col = cel.column();
  switch (e->key()) {
  case Qt::Key_Backspace:
    qDebug() << "table backspace";
    if (cursor.hasSelection()) {
      deleteSelection();
    } else if (col==0 && data()->isRowEmpty(row) && data()->rows()>1) {
      deleteRows(row, 1);
      if (row>0)
	gotoCell(row-1, data()->columns()-1, true);
      else
	normalizeCursorPosition();
    } else if (data()->rows()==1 && col>0
	       && cursor.position()==cel.firstPosition()) {
      qDebug() << "Join with prev";
      // join col with previous
      joinColumnWithNext(col-1);
      cursor.movePosition(TextCursor::Right);
    } else if (data()->isColumnEmpty(col) && data()->columns()>1) {
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
      deleteSelection();
    } else if (col==0 && data()->isRowEmpty(row) && data()->rows()>1) {
      deleteRows(row, 1);
      gotoCell(row, col);
      normalizeCursorPosition();
    } else if (data()->rows()==1 && col<data()->columns()-1
	       && cursor.position()==cel.lastPosition()) {
      // join col with next
      joinColumnWithNext(col);
    } else if (data()->isColumnEmpty(col) && data()->columns()>1) {
      deleteColumns(col, 1);
      gotoCell(row, col);
      normalizeCursorPosition();
    } else if (cursor.position() < cel.lastPosition()) {
      cursor.deleteChar();
    }
    return true;
  case Qt::Key_Tab:
    if (data()->rows()==1 && !shft && !ctrl) {
      splitColumn(col, cursor.position()-cel.firstPosition());
      return true;
    }
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
      if (row>=data()->rows()-1)
	insertRow(row+1);
      gotoCell(row+1, 0, true);
    }
    return true;
  default:
    break;
  }
  return false;
}

bool TableItem::nothingAfterCursor() const {
  TableCell cel(cellAtCursor());
  if (cel.row()<data()->rows()-1)
    return false;
  if (cursor.position()!=cel.lastPosition())
    return false;
  for (int c=cel.column()+1; c<data()->columns(); c++) 
    if (!data()->cell(cel.row(), c).isEmpty())
      return false;
  return true;
}

bool TableItem::tryToPaste(bool /*noparagraphs*/) {
  qDebug() << "Table::trytopaste";
  // following copied from TextItem's trytopaste
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);

  // let's not try to parse html tables right now, although we could
  if (md->hasText()) {
    QString txt = md->text();
    QString mid = txt.mid(1, txt.size()-2);
    if (mid.contains("\t") || mid.contains("\n"))
      return pasteMultiCell(txt);
  }
  if (selectionSpansCells())
    return false;
  else
    return TextItem::tryToPaste(true);
}

void TableItem::tryToCopyCells(class TableCellRange const &rng) const {
  QString html = "<table>";
  QString txt = "";
  for (int r=rng.firstRow(); r<=rng.lastRow(); r++) {
    html += "<tr>";
    for (int c=rng.firstColumn(); c<=rng.lastColumn(); c++) {
      html += "<td>";
      html += toHtml(data()->cellStart(r,c), data()->cellEnd(r,c));
      html += "</td>";
      txt += data()->text().mid(data()->cellStart(r,c),
				data()->cellLength(r,c));
      txt += "\t";
    }
    html += "</tr>";
    txt = txt.left(txt.size()-1) + "\n";
  }
  html += "</table>";
  QClipboard *cb = QApplication::clipboard();
  QMimeData *md = new QMimeData();
  md->setText(txt);
  md->setHtml(html);
  cb->setMimeData(md);
}

void TableItem::deleteSelection() {
  if (!cursor.hasSelection())
    return;
  if (isWholeCellSelected()) {
    TableCellRange rng = selectedCells();
    int r0 = rng.firstRow();
    int c0 = rng.firstColumn();
    int nr = rng.rows();
    int nc = rng.columns();
    qDebug() << r0 << c0 << "+" << nr << nc << "/" << data()->rows() << data()->columns();
    if (nr==int(data()->rows()) && nc==int(data()->columns())) {
      // deleting entire table
      data()->setRows(1);
      data()->setColumns(1);
      data()->setText("\n\n", true);
      emit unicellular(data());
    } else if (nr==int(data()->rows()) && data()->columns()>1) {
      deleteColumns(c0, nc);
      cursor.clearSelection();
    } else if (nc==int(data()->columns()) && data()->rows()>1) {
      deleteRows(r0, nr);
      cursor.clearSelection();
    } else {
      for (int r=r0; r<r0+nr; r++) {
	for (int c=c0; c<c0+nc; c++) {
          int s = data()->cellStart(r,c);
          int e = data()->cellEnd(r,c);
          if (e>s) {
            cursor.setPosition(s);
            cursor.setPosition(e, TextCursor::KeepAnchor);
            cursor.deleteChar();
          }
	}
      }
    }
    normalizeCursorPosition();
  } else {
    cursor.deleteChar();
  }
}

bool TableItem::keyPressWithControl(QKeyEvent *e) {
  if (!(e->modifiers() & Qt::ControlModifier))
    return false;

  TableCellRange rng = selectedCells();
  int r0 = rng.firstRow();
  int c0 = rng.firstColumn();
  int nr = rng.rows();
  int nc = rng.columns();
  int C = data()->columns();
  int R = data()->rows();
  
  switch (e->key()) {
  case Qt::Key_C:
    if (isWholeCellSelected())
      tryToCopyCells(selectedCells());
    else
      tryToCopy();
    return true;
  case Qt::Key_X:
    if (isWholeCellSelected())
      tryToCopyCells(selectedCells());
    else 
      tryToCopy();
    // fall through
  case Qt::Key_Delete: case Qt::Key_Backspace:
    deleteSelection();
    break;
  case Qt::Key_A: 
    if (isWholeCellSelected()) {
      if (nr==R && nc==C) {
	// everything selected; cycle back to just one cell
	selectCell(ctrla_r0, ctrla_c0);
      } else if (nr==R) {
	// column selected -> select table
	TextCursor cur0(document(), data()->cellStart(0,0));
	TextCursor cur1(document(), data()->cellEnd(R-1, C-1));
	cur0.setPosition(cur1.position(), TextCursor::KeepAnchor);
	setTextCursor(cur0);
      } else if (nc==C) {
	// row selected -> select column
	if (ctrla_c0<0)
	  ctrla_c0 = 0;
	else if (ctrla_c0>=C)
	  ctrla_c0 = C-1;
	TextCursor cur0(document(), data()->cellStart(0, ctrla_c0));
	TextCursor cur1(document(), data()->cellEnd(R-1, ctrla_c0));
	cur0.setPosition(cur1.position(), TextCursor::KeepAnchor);
	setTextCursor(cur0);
      } else {
	// less than a row, less than a column -> select row
        if (ctrla_r0<r0)
          ctrla_r0 = r0;
        else if (ctrla_r0>=r0+nr)
          ctrla_r0 = r0+nr-1;
        TextCursor cur0(document(), data()->cellStart(ctrla_r0, 0));
        TextCursor cur1(document(), data()->cellEnd(ctrla_r0, C-1));
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
    if (!cursor.hasSelection())
      tryToPaste();
    return true;
  case Qt::Key_N:
    if (nr==1 && nc==1) // footnote refs cannot span cells
      tryFootnote(e->modifiers() & Qt::ShiftModifier);
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
    //qDebug() << "TableItem::normalizeCursorPosition actually did something";
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
  } else { // why did I have:   if (!selectionSpansCells())   here?
    TextItem::keyPressEvent(e);
  }
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
    setTextCursor(TextCursor(document(), data()->cellEnd(r,c)));
  else
    setTextCursor(TextCursor(document(), data()->cellStart(r,c)));
}

void TableItem::selectCell(int r, int c) {
  gotoCell(r, c);
  cursor.setPosition(data()->cellEnd(r, c), TextCursor::KeepAnchor);
  update();
}

void TableItem::deleteRows(int r0, int N) {
  for (int n=0; n<N; n++)
    data()->deleteRow(r0);

  text->relayout();
  docChange();

  if (data()->rows()==1 && data()->columns()==1)
    emit unicellular(data());
}

void TableItem::deleteColumns(int c0, int N) {
  for (int n=0; n<N; n++)
    data()->deleteColumn(c0);

  text->relayout();
  docChange();

  if (data()->rows()==1 && data()->columns()==1)
    emit unicellular(data());
}

void TableItem::insertRow(int before) {
  data()->insertRow(before);

  text->relayout();
  docChange();  
}

void TableItem::insertColumn(int before) {
  data()->insertColumn(before);

  text->relayout();
  docChange();  
}

QList<TextCursor> TableItem::normalizeSelection() const {
  QList<TextCursor> lst;
  if (selectionSpansCells()) {
    TableCellRange rng = selectedCells();
    for (int r=rng.firstRow(); r<=rng.lastRow(); r++) 
      for (int c=rng.firstColumn(); c<=rng.lastColumn(); c++) 
        lst << cursorSelectingCell(data()->cell(r, c));
  } else {
    lst << cursor;
  }
  return lst;
}

TextCursor TableItem::cursorSelectingCell(TableCell const &cel) const {
  TextCursor curs = cursor;
  curs.setPosition(cel.firstPosition());
  curs.setPosition(cel.lastPosition(), TextCursor::KeepAnchor);
  return curs;
}

bool TableItem::selectionSpansCells() const {
  TableCellRange cr = selectedCells();
  return cr.columns()>1 || cr.rows()>1;
}
  

bool TableItem::isWholeCellSelected() const {
  if (!cursor.hasSelection())
    return false;
  if (selectionSpansCells())
    return true;
  TableCell cel = cellAtCursor();
  TextCursor::Range r = cursor.selectedRange();
  return r.start()==cel.firstPosition() && r.end()==cel.lastPosition();
}

void TableItem::focusInEvent(QFocusEvent *e) {
  emit mustNormalizeCursor();
  TextItem::focusInEvent(e);
}

TableCellRange TableItem::selectedCells() const {
  TextCursor::Range rr = cursor.selectedRange();
  TableCell a = data()->cellAt(rr.start());
  TableCell b = data()->cellAt(rr.end());
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
  return TableCellRange(data(), r0, c0, dr+1, dc+1);
}

TableCell TableItem::cellAtCursor() const {
  return data()->cellAt(cursor.position());
}

void TableItem::paint(QPainter *p,
		      const QStyleOptionGraphicsItem *o, QWidget *w) {
  TextItem::paint(p, o, w);

  // Draw top and bottom lines
  QPen pen(text->color());
  pen.setWidth(1);
  p->setPen(pen);
  QRectF r = text->tightBoundingRect();
  p->drawLine(r.topLeft()+QPointF(1, 1), r.topRight()+QPointF(-1, 1));
  p->drawLine(r.bottomLeft()+QPointF(1, -1), r.bottomRight()+QPointF(-1, -1));
}

void TableItem::representCursor(QList<TransientMarkup> &tmm) const {
  // Represent local or multicell selection as a markup
  if (cursor.hasSelection()) {
    if (selectionSpansCells()) {
      TableCellRange rng = selectedCells();
      for (int r=rng.firstRow(); r<=rng.lastRow(); r++)
        for (int c=rng.firstColumn(); c<=rng.lastColumn(); c++)
          tmm << TransientMarkup(data()->cellStart(r,c),
                                 data()->cellEnd(r,c),
                                 MarkupData::Selected);
    } else {
      tmm << TransientMarkup(cursor.selectionStart(), cursor.selectionEnd(),
                             MarkupData::Selected);
    }
  }
}

bool TableItem::pasteMultiCell(QString txt) {
  if (cursor.hasSelection())
    return false; // simply refuse

  QStringList rows = txt.split("\n");
  while (!rows.isEmpty() && rows.last().isEmpty())
    rows.takeLast();
  if (rows.isEmpty())
    return false;
  int nr = rows.size();

  QList<QStringList> cels;
  foreach (QString const &r, rows)
    cels << r.split("\t");
  int nc = 0;
  foreach (QStringList const &r, cels)
    if (r.size()>nc)
      nc = r.size();

  /* Now we'll insert.
     (1) If there is a large enough empty rectangle in the table, (possibly
         after making the table wider and/or taller), great, we'll fill it.
     (2) If there is a large enough empty space for one row, (possibly
         after making the table wider), we'll use it for our first row
	 and insert empty rows for the rest.
     (3) If there is a large enough empty space for one column, (possibly
         after making the table taller), we'll use it for our first column
	 and insert empty columns for the rest.
     (4) ?
     Is this a good algorithm that would not be surprising in general?
     Remember that we don't have an undo mechanism (yet). So if we screw
     up a table, the user cannot do much to undo the damage.
  */

  /* Let's try that again.
     (1) If nothing is in the way at all, just plonk the new stuff in.
     (2) If nothing is in the way to the right, move rows down that present
         obstacles.
     (3) If nothing is in the way downward, move columns over that present
         obstacles.
     (4) If obstructions occur in both directions, it is really not obvious
         what is the best thing to do. I could simply refuse.
  */
  /* Wait. That's not properly defined. (1) is good, but (2) and (3) are not.
     If a cell presents an obstacle, it presents an obstacle in both directions.
     So a well-defined question is: after how many rows is the left column
     blocked, or after how many columns is the top row blocked.
     I can declare that if blocking occurs first in the top row or left column,
     I'll make space, but if blocking occurs first in any other row or column,
     I'll refuse.
  */

  int firstBlockedColumnTop = nc;
  int firstBlockedColumnOther = nc;
  int firstBlockedRowLeft = nr;
  int firstBlockedRowOther = nr;
  TableCell topLeft = data()->cellAt(cursor.position());
  int NR = data()->rows();
  int NC = data()->columns();
  int r0 = topLeft.row();
  int c0 = topLeft.column();

  // find blocking column on top
  for (int c=0; c<nc; c++) {
    if (c0+c >= NC)
      break;
    if (!data()->isCellEmpty(r0, c0+c)) {
      firstBlockedColumnTop = c;
      break;
    }
  }

  // find blocking column in rest
  for (int r=1; r<nr; r++) {
    if (r0+r >= NR)
      break;
    for (int c=0; c<nc; c++) {
      if (c0+c >= NC)
	break;
      if (firstBlockedColumnOther>c && !data()->isCellEmpty(r0+r, c0+c)) {
	firstBlockedColumnOther = c;
	break;
      }
    }
  }

  // find blocking row on top
  for (int r=0; r<nr; r++) {
    if (r0+r >= NR)
      break;
    if (!data()->isCellEmpty(r0+r, c0)) {
      firstBlockedRowLeft = r;
      break;
    }
  }

  // find blocking row in rest
  for (int c=1; c<nc; c++) {
    if (c0+c >= NC)
      break;
    for (int r=0; r<nr; r++) {
      if (r0+r >= NR)
	break;
      if (firstBlockedRowOther>r && !data()->isCellEmpty(r0+r, c0+c)) {
	firstBlockedRowOther = r;
	break;
      }
    }
  }

  qDebug() << "R" << nr << NR << firstBlockedRowLeft << firstBlockedRowOther;
  qDebug() << "C" << nc << NC << firstBlockedColumnTop << firstBlockedColumnOther;
  if (firstBlockedRowLeft>=nr && firstBlockedRowOther>=nr
      && firstBlockedColumnTop>=nc && firstBlockedColumnOther>=nc) {
    ; // easy
  } else if (firstBlockedColumnTop>=nc
	     && firstBlockedRowLeft<=firstBlockedRowOther) {
    // shift rows down
    for (int k=0; k<nr-firstBlockedRowLeft; k++)
      insertRow(r0+firstBlockedRowLeft);
  } else if (firstBlockedRowLeft>=nr
	     && firstBlockedColumnTop<=firstBlockedColumnOther) {
    // shift columns over
    for (int k=0; k<nc-firstBlockedColumnTop; k++)
      insertColumn(c0+firstBlockedColumnTop); 
  } else {
    // refuse
    return false;
  }

  // expand table
  while (data()->columns() < c0+nc)
    insertColumn(data()->columns());
  while (data()->rows() < r0+nr)
    insertRow(data()->rows());

  // Finally. All target cells are empty. Let's do it.
  for (int r=0; r<nr; r++) {
    for (int c=0; c<nc; c++) {
      cursor.setPosition(data()->cellStart(r0+r, c0+c));
      if (cels[r].size()>c)
	cursor.insertText(cels[r][c]);
    }
  }
  update();
  return true;
}

void TableItem::joinColumnWithNext(int col) {
  int pos = data()->cellEnd(0, col);
  data()->joinColumnWithNext(col);
  setTextCursor(TextCursor(document(), pos));
  text->relayout();
  docChange();

  if (data()->rows()==1 && data()->columns()==1)
    emit unicellular(data());
}

void TableItem::splitColumn(int col, int offset) {
  data()->splitColumn(col, offset);
  int pos = data()->cellEnd(0, col);
  setTextCursor(TextCursor(document(), pos + 1));
  text->relayout();
  docChange();
}
