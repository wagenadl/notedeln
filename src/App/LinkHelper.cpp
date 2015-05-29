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
    newMarkup(md);
}

LinkHelper::~LinkHelper() {
}

MarkupData *LinkHelper::findMarkup(QPointF p) const {
  int pos = item->pointToPos(p, true);
  if (pos<0)
    return 0;
  return item->markupAt(pos, MarkupData::Link);
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

void LinkHelper::newMarkup(MarkupData *md) {
  if (md->style()==MarkupData::Link)
    links[md] = new OneLink(md, item);
}

void LinkHelper::removeMarkup(MarkupData *md) {
  if (links.contains(md)) {
    links[md]->deleteLater();
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
