// Scenes/TOCScene.cpp - This file is part of eln

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

// TOCScene.C

#include "TOCScene.H"
#include "TOC.H"
#include "Roman.H"
#include <QGraphicsLineItem>
#include "TOCItem.H"
#include <QDebug>

TOCScene::TOCScene(TOC *data, QObject *parent):
  BaseScene(data, parent),
  data(data) {
  connect(data, SIGNAL(mod()), this, SLOT(tocChanged()));
}

TOCScene::~TOCScene() {
}

void TOCScene::populate() {
  BaseScene::populate();
  rebuild();
  BaseScene::populate(); // do it again; we have new sheet count
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
    connect(i, SIGNAL(vboxChanged()), SLOT(itemChanged()));
    connect(i, SIGNAL(clicked(int)), SIGNAL(pageNumberClicked(int)));
    
    lines.append(addLine(0, 0, style().real("page-width"), 0,
			 QPen(QBrush(style().color("toc-line-color")),
			      style().real("toc-line-width"))));
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

  for (int k=0; k<items.size(); k++) {
    TOCItem *i = items[k];
    double h = i->childrenBoundingRect().height();
    if (y+h > y1) {
      y = y0;
      sheet ++;
    }
    i->setPos(QPointF(0, y + sheet*ph));
    y += h;
    lines[k]->setLine(0, y + sheet*ph, pw, y + sheet*ph);
  }

  nSheets = sheet+1;
}
QString TOCScene::pgNoToString(int n) const {
  return Roman(n).lc();
}
  
void TOCScene::makeContdItems() {
  // Simply don't make them??
  BaseScene::makeContdItems();
  for (int n=1; n<nSheets; n++)
    contdItems[n]->setPos(4, style().real("page-height")*n
			  + style().real("margin-top"));
}
