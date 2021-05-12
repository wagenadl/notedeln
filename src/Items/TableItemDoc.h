// Items/TableItemDoc.h - This file is part of NotedELN

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

// TableItemDoc.h

#ifndef TABLEITEMDOC_H

#define TABLEITEMDOC_H

#include "TextItemDoc.h"

class TableItemDoc: public TextItemDoc {
  Q_OBJECT;
public:
  TableItemDoc(class TableData *data, QObject *parent=0);
  virtual ~TableItemDoc() { }
  virtual void relayout(bool preserveWidths=false);
  virtual int firstPosition() const;
  virtual int lastPosition() const;
  virtual int find(QPointF p, bool strict=true) const;
  virtual QString selectedText(int start, int end) const;
  virtual QRectF tightBoundingRect() const;
  QPointF cellLocation(int r, int c) const;
  QRectF cellBoundingRect(int r, int c) const;
protected:
  void buildLinePos();
protected:
  TableData const *table() const;
  TableData *table();
};

#endif
