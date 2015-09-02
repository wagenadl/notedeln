// Items/SearchResItem.h - This file is part of eln

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

// SearchResItem.H

#ifndef SEARCHRESITEM_H

#define SEARCHRESITEM_H

#include "TOCItem.h"
#include "Search.h"

class SearchResItem: public TOCItem {
  Q_OBJECT;
public:
  SearchResItem(class TOCEntry *data, class BaseScene *parent);
  virtual ~SearchResItem();
  DATAACCESS(TOCEntry);
  void reset();
  void addResult(SearchResult const &res, QGraphicsItem *parent);
signals:
  void clicked(int, Qt::KeyboardModifiers, QString); // page number; uuid
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  int refineBreak(QString s, int idx);
  int decentBreak(QString s, int first, int last);
  void fillText(class QTextDocument *doc, SearchResult const &res);
private:
  QList<class QGraphicsTextItem *> items;
  QList<double> ytop;
  QList<int> pgno;
  QList<QString> uuid;
};

#endif
