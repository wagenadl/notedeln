// Data/TableData.H - This file is part of eln

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

// TableData.H

#ifndef TABLEDATA_H

#define TABLEDATA_H

#include "TextData.h"
#include "TableCell.h"
#include <QVector>

class TableData: public TextData {
  Q_OBJECT;
  Q_PROPERTY(int nr READ rows WRITE setRows)
  Q_PROPERTY(int nc READ columns WRITE setColumns)
public:
  TableData(Data *parent=0);
  virtual ~TableData();
  void setRows(int r);
  void setColumns(int c);
  virtual void setText(QString const &, bool hushhush=false);
  int rows() const;
  int columns() const;
  int cellLength(int r, int c) const;
  int cellStart(int r, int c) const;
  int cellEnd(int r, int c) const;
  QString cellContents(int r, int c) const;
  bool isRowEmpty(int r) const;
  bool isColumnEmpty(int c) const;
  bool isCellEmpty(int r, int c) const;
  int lastNonEmptyCellInRow(int r) const; // 0 if none
  virtual bool isEmpty() const;
  virtual QVector<int> const &lineStarts() const;
  bool isValid() const;
  /* setRows, setColumns, and setText automatically recalculate cell starts
     and lengths, but only if everything is consistent.
     This function reports on consistency.
  */
  TableCell cellAt(int pos) const;
  TableCell cell(int c, int r) const;
  int rc2index(int r, int c) const;
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
protected:
  void recalculate();
  int countCells() const;
protected:
  int nr;
  int nc;
private:
  virtual void setLineStarts(QVector<int> const &);
private:
  QVector<int> lengths;
  QVector<int> starts;
  bool valid;
};

#endif
