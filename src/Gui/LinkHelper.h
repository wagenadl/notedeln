// App/LinkHelper.h - This file is part of eln

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

// LinkHelper.h

#ifndef LINKHELPER_H

#define LINKHELPER_H

#include <QObject>
#include <QMap>
#include <QPointF>

class LinkHelper: public QObject {
public:
  LinkHelper(class TextItem *parent);
  virtual ~LinkHelper();
  bool mousePress(class QGraphicsSceneMouseEvent *);
  bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  void mouseMove(class QGraphicsSceneHoverEvent *);
public:
  void updateMarkup(class MarkupData *);
  class OneLink *newMarkup(MarkupData *, bool trulyNew=true);
  void removeMarkup(MarkupData *);
  void updateAll();
  OneLink *linkFor(MarkupData *);
private:
  MarkupData *findMarkup(QPointF) const;
  void mouseCore(QPointF);
  void perhapsLeave(MarkupData *);
private:
  TextItem *item;
  QMap<class MarkupData *, class OneLink *> links;
  class OneLink *current;
};

#endif
