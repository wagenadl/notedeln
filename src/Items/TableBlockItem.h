// Items/TableBlockItem.H - This file is part of eln

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

// TableBlockItem.H

#ifndef TABLEBLOCKITEM_H

#define TABLEBLOCKITEM_H

#include "TextBlockItem.h"
#include "TableBlockData.h"

class TTICreator: public TICreator {
public:
  virtual ~TTICreator();
  virtual class TextItem *create(TextData *data, Item *parent,
				 class TextItemDoc *altdoc=0) const;
};

class TableBlockItem: public TextBlockItem {
  Q_OBJECT;
public:
  TableBlockItem(TableBlockData *data, Item *parent=0);
  virtual ~TableBlockItem();
  DATAACCESS(TableBlockData);
  class TableItem *table();
signals:
  void unicellular(TableData *); // emitted when we are left as just one cell
private:
  class TableItem *item_;
};

#endif
