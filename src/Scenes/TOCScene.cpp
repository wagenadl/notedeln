// Scenes/TOCScene.cpp - This file is part of NotedELN

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

// TOCScene.C

#include "TOCScene.h"
#include "TOC.h"
#include "Roman.h"
#include "SheetScene.h"
#include <QGraphicsLineItem>
#include "TOCItem.h"
#include <QDebug>
#include "PageView.h"
#include "EventView.h"

TOCScene::TOCScene(TOC *data, QObject *parent):
  BaseScene(data, parent),
  data(data) {
  setContInMargin();
  connect(data, &TOC::mod, this, &TOCScene::tocChanged);
}

TOCScene::~TOCScene() {
}

void TOCScene::populate() {
  BaseScene::populate();
  rebuild();
}

QString TOCScene::title() const {
  return "Table of Contents";
}

void TOCScene::tocChanged() {
  if (data->entries().size() != items.size()) {
    rebuild();
  }
}

void TOCScene::itemChanged() {
  relayout();
}

void TOCScene::rebuild() {
  foreach (TOCItem *i, items)
    i->deleteLater();
  foreach (QGraphicsLineItem *l, lines)
    delete l;
  items.clear();
  lines.clear();
  
  foreach (TOCEntry *e, data->entries()) {
    TOCItem *i = new TOCItem(e, this);
    items.append(i);
    connect(i, &TOCItem::vboxChanged, this, &TOCScene::itemChanged);
    connect(i, &TOCItem::clicked,
	    this, &TOCScene::pageNumberClicked);

    QGraphicsLineItem *l
      = new QGraphicsLineItem(0, 0, style().real("page-width"), 0);
    l->setParentItem(i);
    l->setPen(QPen(QBrush(style().color("toc-line-color")),
		   style().real("toc-line-width")));
    lines.append(l);
  }
  relayout();
}

void TOCScene::relayout() {
  int sheet = 0;
  double y0 = style().real("margin-top");
  double pw = style().real("page-width");
  double ph = style().real("page-height");
  double y1 = ph - style().real("margin-bottom");
  double y = y0;

  page2sheet.clear();
  
  for (int k=0; k<items.size(); k++) {
    TOCItem *i = items[k];
    double h = i->childrenBoundingRect().height();
    if (y+h > y1) {
      y = y0;
      sheet++;
    }
    page2sheet[i->data()->startPage()] = sheet;
    if (sheet>=sheetCount())
      setSheetCount(sheet+1);
    sheets[sheet]->addItem(i);
    i->setPos(QPointF(0, y));
    y += h;
    lines[k]->setLine(0, h, pw, h);
  }
  
  if (sheet+1 != sheetCount())
    setSheetCount(sheet+1);
}

QString TOCScene::pgNoToString(int n) const {
  return Roman(n).lc();
}

void TOCScene::pageNumberClicked(int n, Qt::KeyboardModifiers m) {
  PageView *ev = EventView::eventView();
  qDebug() << "TOCScene::pagenumberclicked. event view is " << ev;
  if (ev)
    ev->pageNumberClick(n, m);
}

int TOCScene::sheetForPage(int pg) const {
  while (pg>0) {
    if (page2sheet.contains(pg))
      return page2sheet[pg];
    --pg;
  }
  return 0;
}

