// Items/DefaultingTextItem.h - This file is part of eln

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

// DefaultingTextItem.h

#ifndef DEFAULTINGTEXTITEM_H

#define DEFAULTINGTEXTITEM_H

#include "TextItem.h"
#include <QPainter>
#include "TextItemDoc.h"

class DefaultingTextItem: public TextItem {
public:
  DefaultingTextItem(TextData *data, Item *parent=0,
		     bool noFinalize=false,
		     class TextItemDoc *altdoc=0);
  virtual ~DefaultingTextItem();
  void setDefaultText(QString);
protected:
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
signals:
  void textChanged();
private:
  QString dflt_text;
};

#endif
