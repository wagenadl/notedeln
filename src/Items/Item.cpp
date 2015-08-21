// Items/Item.cpp - This file is part of eln

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

// Item.C

#include "Item.h"
#include "EntryScene.h"
#include <QDebug>
#include "Notebook.h"
#include "Cursors.h"
#include "LateNoteItem.h"
#include "FootnoteItem.h" // for debug
#include "EntryScene.h"
#include <QGraphicsSceneMouseEvent>
#include "DragLine.h"
#include "App.h"
#include "GfxData.h"
#include "GfxNoteData.h"
#include "Assert.h"
#include <QGraphicsDropShadowEffect>
#include "Mode.h"
#include "BlockItem.h"
#include "SheetScene.h"

Item::Item(Data *d, Item *parent): QGraphicsObject(parent), d(d) {
  ASSERT(d);
  writable = false;
  setAcceptHoverEvents(true);
  //  setCursor(defaultCursorShape());
}

Item::~Item() {
}

void Item::deleteLater() {
  ASSERT(d);
  d = 0;
  QGraphicsObject::setParentItem(0);
  if (scene()) 
    scene()->removeItem(this);
  QGraphicsObject::deleteLater();
}

Style const &Item::style() const {
  ASSERT(d);
  Notebook *n = d->book();
  if (n)
    return n->style();
  qDebug() << "Item" << this << "not connected to notebook";
  // This appears to happens during item deletion. horrible, but true.
  return Style::defaultStyle();
}

QVariant Item::style(QString k) const {
  return style()[k];
}

bool Item::isWritable() const {
  return writable;
}

void Item::makeWritableNoRecurse() {
  ASSERT(d);
  writable = true;
}

void Item::makeWritable() {
  ASSERT(d);
  writable = true;
  foreach (Item *i, allChildren())
    i->makeWritable();
}

bool Item::hasMode() const {
  SheetScene *ss = dynamic_cast<SheetScene*>(scene());
  if (!ss)
    return false;
  return ss->mode() ? true : false;
}

Mode *Item::mode() const {
  SheetScene *ss = dynamic_cast<SheetScene*>(scene());
  ASSERT(ss);
  Mode *m = ss->mode();
  //  if (!m)
  //    return new Mode(true); // ouch; just for now
  ASSERT(m);
  return m;
}

Item *Item::create(Data *d, Item *parent) {
  ASSERT(d);
  if (creators().contains(d->type()))
    return creators()[d->type()](d, parent);
  qDebug() << "Item::create: No creator for " << d->type();
  ASSERT(0);
  return 0;
}

QMap<QString, Item *(*)(Data *, Item *)> &Item::creators() {
  static QMap<QString, Item *(*)(Data *, Item *)> m;
  return m;
}
  
QRectF Item::netBounds() const {
  QRectF b = boundingRect();
  foreach (Item *i, allChildren()) 
    if (!i->excludeFromNet())
      b |= i->mapRectToParent(i->netBounds());
  return b;
}

bool Item::excludeFromNet() const {
  return false;
}

Item *Item::parent() const {
  return dynamic_cast<Item*>(QGraphicsObject::parentItem());
}

QList<Item*> Item::allChildren() const {
  return children<Item>();
}

Qt::CursorShape Item::defaultCursorShape() {
  return Qt::ArrowCursor;
}

Qt::CursorShape Item::cursorShape() const {
  return defaultCursorShape();
}

bool Item::changesCursorShape() const {
  return false;
}

GfxNoteItem *Item::newNote(QPointF p0, QPointF p1, bool late) {
  ASSERT(d);
  GfxNoteItem *n = (late || !data()->isRecent())
    ? LateNoteItem::newNote(p0, p1, this)
    : GfxNoteItem::newNote(p0, p1, this);
  if (ancestralBlock())
    ancestralBlock()->sizeToFit();
  return n;
}

GfxNoteItem *Item::createNote(QPointF p0, bool late) {
  ASSERT(d);
  QPointF p1 = mapFromScene(DragLine::drag(scene(), mapToScene(p0), style()));
  return newNote(p0, p1, late);
}

bool Item::shouldGlow() const {
  return dynamic_cast<GfxData*>(d)
    || (dynamic_cast<TextData*>(d) &&
        dynamic_cast<GfxData*>(d->parent()));
}

void Item::removeGlow() {
  setGraphicsEffect(0);
}

void Item::modeChangeUnderCursor() {
  if (mode()->mode()==Mode::MoveResize)
    perhapsCreateGlow();
  else
    removeGlow();
}

void Item::perhapsCreateGlow() {
  ASSERT(d);
  if (writable && shouldGlow() && mode()->mode()==Mode::MoveResize) {
    QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect;
    eff->setColor(QColor("#00ff33"));
    eff->setOffset(QPointF(0, 0));
    eff->setBlurRadius(4);
    setGraphicsEffect(eff);
  }
}  

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  perhapsCreateGlow();
  if (changesCursorShape())
    setCursor(Cursors::refined(cursorShape()));
  e->accept();
}

void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  ASSERT(d);
  removeGlow();
  e->accept();
}


BlockItem const *Item::ancestralBlock() const {
  return parent() ? parent()->ancestralBlock() : 0;
}

BlockItem *Item::ancestralBlock() {
  return parent() ? parent()->ancestralBlock() : 0;
}

void Item::setScale(qreal f) {
  QGraphicsObject::setScale(f);
}
