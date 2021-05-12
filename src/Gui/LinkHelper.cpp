// App/LinkHelper.cpp - This file is part of NotedELN

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

// LinkHelper.cpp

#include "LinkHelper.h"
#include "TextItem.h"
#include "OneLink.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>

LinkHelper::LinkHelper(TextItem *parent): QObject(parent) {
  item = parent;
  current = 0;
  foreach (MarkupData *md, parent->data()->children<MarkupData>())
    newMarkup(md, false);
}

LinkHelper::~LinkHelper() {
}

MarkupData *LinkHelper::findMarkup(QPointF p) const {
  int pos = item->pointToPos(p, true);
  if (pos<0)
    return 0;
  return item->data()->markupAt(pos, MarkupData::Link);
}

void LinkHelper::perhapsLeave(MarkupData *md) {
  if (links.contains(md)) {
    OneLink *nwlink = links[md];
    if (nwlink==current)
      return;
  }
  if (current)
    current->leave();
  current = 0;
}

void LinkHelper::mouseCore(QPointF p) {
  MarkupData *md = findMarkup(p);
  perhapsLeave(md);
  if (md==0)
    return;
  if (links.contains(md)) 
    current = links[md];
}

bool LinkHelper::mousePress(QGraphicsSceneMouseEvent *e) {
  mouseCore(e->pos());
  return current ? current->mousePress(e) : false;
}

bool LinkHelper::mouseDoubleClick(QGraphicsSceneMouseEvent *e) {
  mouseCore(e->pos());
  return current ? current->mouseDoubleClick(e) : false;
}

void LinkHelper::mouseMove(QGraphicsSceneHoverEvent *e) {
  OneLink *oc = current;
  mouseCore(e->pos());
  if (current && current!=oc)
    current->enter(e);
}

OneLink *LinkHelper::newMarkup(MarkupData *md, bool trulyNew) {
  if (md->style()==MarkupData::Link) {
    links[md] = new OneLink(md, item, trulyNew);
    return links[md];
  } else {
    return NULL;
  }
}

void LinkHelper::removeMarkup(MarkupData *md) {
  if (links.contains(md)) {
    OneLink *lnk = links[md];
    if (current==lnk)
      current = 0;
    lnk->deleteLater();
    links.remove(md);
  }
}

void LinkHelper::updateMarkup(MarkupData *md) {
  if (links.contains(md))
    links[md]->update();
}

void LinkHelper::updateAll() {
  foreach (OneLink *l, links)
    l->update();
}

OneLink *LinkHelper::linkFor(MarkupData *md) {
  if (links.contains(md))
    return links[md];
  else
    return NULL;
}
