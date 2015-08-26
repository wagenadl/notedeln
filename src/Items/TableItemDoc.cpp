// TableItemDoc.cpp

#include "TableItemDoc.h"
#include "TableData.h"
#include "TextItemDocData.h"
#include <QDebug>
#include "Assert.h"

TableItemDoc::TableItemDoc(class TableData *data, QObject *parent):
  TextItemDoc(data, parent) {
  /* Traditionally, tables have had cell lengths stored rather than
     line starts. While this is silly now, we'll stick with it for
     file format compatibility. */
  int C = data->columns();
  int R = data->rows();
  d->linestarts.resize(C*R);
  for (int r=0; r<R; r++)
    for (int c=0; c<C; c++)
      d->linestarts[c+C*r] = data->cellStart(r, c);
}

void TableItemDoc::relayout(bool preserveWidths) {
  if (!preserveWidths)
    d->forgetWidths();
  d->linestarts = table()->lineStarts();
  buildLinePos();
}

void TableItemDoc::buildLinePos() {
  int C = table()->columns();
  int R = table()->rows();
  QVector<double> columnWidth(C, 9.0);  // minimum column width = 9 pt
  QVector<double> const &cw = d->charWidths();
  double right = 0;
  for (int r=0; r<R; r++) {
    for (int c=0; c<C; c++) {
      int pos = table()->cellStart(r, c);
      int len = table()->cellLength(r, c);
      double w = 0;
      for (int k=0; k<len; k++)
	w += cw[pos+k];
      if (w>columnWidth[c])
	columnWidth[c] = w;
    }
  }
  d->linepos.resize(C*R);
  double x0 = 12;
  double y0 = 6;
  double ascent = d->fonts().metrics(MarkupStyles())->ascent();
  for (int r=0; r<R; r++) {
    double x = 4.5 + x0;
    double y = y0 + 4.0 + r*d->lineheight + ascent;
    for (int c=0; c<C; c++)  {
      d->linepos[c+C*r] = QPointF(x, y);
      x += columnWidth[c] + 9.0; // margin
      right = x;
    }
  }
  d->br = QRectF(QPointF(x0 - 4, 0),
		 QPointF(right - 1, R*d->lineheight + 4 + 2*y0));
}

TableData const *TableItemDoc::table() const {
  return dynamic_cast<TableData const*>(d->text);
}

TableData *TableItemDoc::table() {
  return dynamic_cast<TableData*>(d->text);
}


int TableItemDoc::firstPosition() const {
  return 1;
}

int TableItemDoc::lastPosition() const {
  return d->text->text().size() - 1;
}

QPointF TableItemDoc::cellLocation(int r, int c) const {
  int idx = table()->rc2index(r, c);
  return d->linepos[idx];
}

QRectF TableItemDoc::cellBoundingRect(int r, int c) const {
  double ascent = d->fonts().metrics(MarkupStyles())->ascent();
  double height = d->lineheight;
  int C = table()->columns();
  QPointF topLeft = cellLocation(r, c) - QPointF(4.5, ascent + 2);
  QPointF bottomRight = (c+1<C)
    ? cellLocation(r, c + 1) + QPointF(-4.5, height - ascent - 2)
    : QPointF(d->br.right(), topLeft.y() + height);
  return QRectF(topLeft, bottomRight);
}

int TableItemDoc::find(QPointF xy, bool /*strict*/) const {
  ASSERT(!d->linepos.isEmpty());
  int R = table()->rows();
  int C = table()->columns();
  for (int r=0; r<R; r++) {
    for (int c=0; c<C; c++) {
      if (cellBoundingRect(r, c).contains(xy)) {
	double x = xy.x();
	int pos = table()->cellStart(r, c);
	int npos = pos + table()->cellLength(r, c);
	double x0 = cellLocation(r, c).x();
	QVector<double> const &charw = d->charWidths();
	while (pos<npos) {
	  double x1 = x0 + charw[pos];
	  if (x0+x1 >= 2*x)
	    return pos;
	  pos++;
	  x0 = x1;
	}
	return pos;
      }
    }
  }
  return -1;
}

QString TableItemDoc::selectedText(int start, int end) const {
  TableCellRange rng(table(), start, end);
  if (rng.isSingleCell())
    return TextItemDoc::selectedText(start, end);

  QStringList texts;
  for (int r=rng.firstRow(); r<=rng.lastRow(); r++) 
    for (int c=rng.firstColumn(); c<=rng.lastColumn(); c++) 
      texts << table()->cellContents(r, c);
  return texts.join("\n");
}

QRectF TableItemDoc::tightBoundingRect() const {
  return boundingRect().adjusted(3, 5, -3, -5);
}

