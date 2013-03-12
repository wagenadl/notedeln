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
  sheetNos.clear();
  
  foreach (TOCEntry *e, data->entries()) {
    TOCItem *i = new TOCItem(e, this);
    items.append(i);
    connect(i, SIGNAL(vboxChanged()), SLOT(itemChanged()));
    connect(i, SIGNAL(clicked(int)), SIGNAL(pageNumberClicked(int)));
    
    sheetNos.append(0);
    lines.append(addLine(0, 0, style().real("page-width"), 0,
			 QPen(QBrush(style().color("toc-line-color")),
			      style().real("toc-line-width"))));
  }
  relayout();
}

void TOCScene::relayout() {
  int sheet = 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = y0;

  for (int k=0; k<items.size(); k++) {
    TOCItem *i = items[k];
    double h = i->childrenBoundingRect().height();
    if (y+h > y1) {
      y = y0;
      sheet ++;
    }
    i->setPos(QPointF(0, y));
    sheetNos[k] = sheet;
    y += h;
    lines[k]->setLine(0, y, style().real("page-width"), y);
  }

  nSheets = sheet+1;
  gotoSheet(iSheet);
}

void TOCScene::gotoSheet(int i) {
  BaseScene::gotoSheet(i);
  for (int n=0; n<items.size(); n++) {
    items[n]->setVisible(sheetNos[n] == i);
    lines[n]->setVisible(sheetNos[n] == i);
  }
}

QString TOCScene::pgNoToString(int n) const {
  return Roman(n).lc();
}
  
void TOCScene::makeContdItems() {
  // simply don't make tem
}
