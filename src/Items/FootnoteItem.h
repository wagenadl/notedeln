// Items/FootnoteItem.H - This file is part of NotedELN

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

// FootnoteItem.H

#ifndef FOOTNOTEITEM_H

#define FOOTNOTEITEM_H

#include "TextBlockItem.h"
#include "FutileMovementInfo.h"
#include "FootnoteData.h"

class FootnoteItem: public TextBlockItem {
  Q_OBJECT;
public:
  FootnoteItem(FootnoteData *data, Item *parent);
  virtual ~FootnoteItem();
  DATAACCESS(FootnoteData);
  class QGraphicsTextItem *tag();
  void setTagText(QString); // updates data
  QString tagText() const;
  bool setAutoContents();
  /* Note that, as a FootnoteItem is a TextBlockItem, it will return
     itself as "ancestralBlock", rather than the block to which it
     is attached.
     Also note that the current code base does not call setBaseScene
     on foot notes. Accordingly, baseScene() will usually return 0.
  */
private:
  void updateTag();
private slots:
  void abandon();
private:
  QGraphicsTextItem *tag_;
};

#endif
