// TableItemDoc.cpp

#include "TableItemDoc.h"

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
  if (!preserveWidth)
    d->forgetWidths();
  buildLinePos();
}

void TableItemDoc::buildLinePos() {
  int C = table()->columns();
  int R = table()->rows();
  QVector<double> columnWidth(C, 9.0);  // minimum column width = 9 pt
  QVector<double> const &cw = d->charWidths();
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
  double ascent = d->fonts().metrics(MarkupStyles())->ascent();
  for (int r=0; r<R; r++) {
    double x = 4.0 + 4.5;
    double y = 4.0 + r*d->lineHeight + ascent;
    for (int c=0; c<C; c++)  {
      d->linepos[c+C*r] = QPointF(x, y);
      x += columnWidth[c] + 9.0; // margin
    }
  }
}

TableData *TableItemDoc::table() {
  return dynamic_cast<TableData*>(d->text);
}

