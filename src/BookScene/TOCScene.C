// TOCScene.C

#include "TOCScene.H"
#include "TOC.H"
#include "Roman.H"
#include <QGraphicsLineItem>
#include "TOCItem.H"

TOCScene::TOCScene(TOC *data, QObject *parent):
  BaseScene(data, parent),
  data(data) {
}

TOCScene::~TOCScene() {
}

void TOCScene::populate() {
  BaseScene::populate();
  rebuild();
  leftMarginItem->hide();
}

QString TOCScene::title() const {
  return "Table of Contents";
}

void TOCScene::rebuild() {
  foreach (TOCItem *i, items)
    delete i;
  items.clear();
  sheetNos.clear();

  int sheet = 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = y0;
  
  foreach (TOCEntry *e, data->entries()) {
    TOCItem *i = new TOCItem(e, this);
    QRectF bb = i->netBoundingRect();
    double h = bb.height();
    if (y+h > y1) {
      y = y0;
      sheet ++;
    }
    i->setPos(i->pos() + QPointF(0, y-bb.top()));
    items.append(i);
    sheetNos.append(sheet);
  }
  nSheets = sheet+1;
  gotoSheet(iSheet);
}

void TOCScene::gotoSheet(int i) {
  BaseScene::gotoSheet(i);
  for (int n=0; n<items.size(); n++)
    items[n]->setVisible(sheetNos[n] == i);
}

QString TOCScene::pgNoToString(int n) const {
  return Roman(n).lc();
}
  
