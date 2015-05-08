// LinkHelper.cpp

#include "LinkHelper.h"
#include "TextItem.h"
#include "OneLink.h"

LinkHelper::LinkHelper(class TextItem *parent): QObject(parent) {
  item = parent;
  current = 0;
}

LinkHelper::~LinkHelper() {
}

MarkupData *LinkHelper::findMarkup(QGraphicsSceneMouseEvent *e) const {
  int pos = item->pointToPos(e->pos());
  if (pos<0)
    return 0;
  return item->markup(pos, MarkupData::Link);
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

void LinkHelper::mouseCore(QGraphicsSceneMouseEvent *e) {
  MarkupData *md = findMarkup(e);
  perhapsLeave(md);
  if (md==0)
    return false;
  if (links.contains(md) && !links[md].isValid()) {
    /* It is possible that a markup data got deleted and that a new markup
       data got constructed at the same address. In that case, the old map
       entry should be dropped.
    */
    links[md]->deleteLater();
    links.remove(md);
  }
  if (!links.contains(md))
    links[md] = new OneLink(md, item);
  current = links[md];
}

bool LinkHelper::mousePress(QGraphicsSceneMouseEvent *e) {
  mouseCore(e);
  return current ? current->mousePress(e) : false;
}

bool LinkHelper::mouseDoubleClick(QGraphicsSceneMouseEvent *e) {
  mouseCore(e);
  return current ? current->mouseDoubleClick(e) : false;
}

void LinkHelper::mouseMove(QGraphicsSceneMouseEvent *e) {
  OneLink *oc = current;
  mouseCore(e);
  if (current && current!=oc)
    current->enter();
}
