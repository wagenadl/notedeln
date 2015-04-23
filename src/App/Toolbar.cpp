// App/Toolbar.cpp - This file is part of eln

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

// Toolbar.C

#include "Toolbar.h"
#include <QDebug>
#include "Assert.h"
#include "ToolItem.h"
#include <QTimerEvent>

#define TOOLGRID 31.5
#define TOOLOFFSET 2

Toolbar::Toolbar(QGraphicsItem *parent): QGraphicsObject(parent) {
  orient = Qt::Vertical;
  selEna = true;
}

Toolbar::~Toolbar() {
}
Qt::Orientation Toolbar::orientation() const {
  return orient;
}

void Toolbar::enableSelect(bool t) {
  if (t) 
    selEna = true;
  else
    disableSelect();
}

void Toolbar::disableSelect() {
  select("");
  selEna = false;
  update();
}

bool Toolbar::isSelectEnabled() const {
  return selEna;
}

void Toolbar::setOrientation(Qt::Orientation o) {
  orient = o;
  arrangeTools();
}

void Toolbar::addTool(QString id, ToolItem *item) {
  ASSERT(!tools.contains(id));
  item->setParentItem(this);
  tools[id] = item;
  revmap[item] = id;
  ids.append(id);
  connect(item, SIGNAL(destroyed(QObject*)), SLOT(childGone()));
  connect(item, SIGNAL(leftClick()), SLOT(leftClicked()));
  connect(item, SIGNAL(rightClick()), SLOT(rightClicked()));
  connect(item, SIGNAL(release()), SLOT(released()));
  arrangeTools();
}
    
QString Toolbar::selection() const {
  return sel;
}

void Toolbar::select(QString s) {
  if (sel==s)
    return;
  if (!sel.isEmpty())
    tools[sel]->setSelected(false);

  if (tools.contains(s)) {
    sel = s;
    tools[sel]->setSelected(true);
  } else {
    sel = "";
    if (s!="")
      qDebug() << "Toolbar: selecting nonexistent tool" << s;
  }
  emit selectionChanged(sel);
}

void Toolbar::released() {
  if (!selEna)
    select("");
}

void Toolbar::leftClicked() {
  ToolItem *t = dynamic_cast<ToolItem*>(sender());
  if (t && revmap.contains(t)) {
    QString id = revmap[t];
    doLeftClick(id);
    select(id);
    if (!selEna)
      startTimer(100); // will deslect after 100 ms
  } else {
    qDebug() << "Toolbar: left click on unknown tool";
  }
}

void Toolbar::timerEvent(QTimerEvent *e) {
  killTimer(e->timerId());
  select("");
}

void Toolbar::rightClicked() {
  ToolItem *t = dynamic_cast<ToolItem*>(sender());
  if (t && revmap.contains(t)) {
    QString id = revmap[t];
    doRightClick(id);
    emit rightClick(id);
  } else {
    qDebug() << "Toolbar: right click on unknown tool";
  }
}
void Toolbar::childGone() {
  ToolItem *t = dynamic_cast<ToolItem*>(sender());
  if (t && revmap.contains(t)) {
    QString id = revmap[t];
    if (sel==id)
      sel="";
    revmap.remove(t);
    tools.remove(id);
    ids.removeOne(id);
    arrangeTools();
  }
}

void Toolbar::arrangeTools() {
  double y = TOOLOFFSET;
  double x = TOOLOFFSET;
  foreach (QString id, ids) {
    tools[id]->setPos(x, y);
    if (orient==Qt::Horizontal)
      x += TOOLGRID;
    else
      y += TOOLGRID;
  }
}

QRectF Toolbar::boundingRect() const {
  return QRectF();
}

void Toolbar::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {
}

void Toolbar::doLeftClick(QString) {
}

void Toolbar::doRightClick(QString) {
}

