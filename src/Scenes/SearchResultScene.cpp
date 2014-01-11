// SearchResultScene.cpp

#include "SearchResultScene.H"
#include "TOCEntry.H"

SearchResultScene::SearchResultScene(QString title, QList<SearchResult> results,
                                     Data *data, QObject *parent):
  BaseScene(data, parent), ttl(title), results(results) {
}

SearchResultScene::~SearchResultScene() {
}

void SearchResultScene::update(QList<SearchResult> res) {
  results = res;
  populate();
}

void SearchResultScene::populate() {
  BaseScene::populate();
  foreach (QGraphicsItem *gi, items)
    delete gi;
  foreach (QGraphicsItem *gi, lines)
    delete gi;
  foreach (TOCData *d, mocktoc)
    delete d;
  items.clear();
  sheetnos.clear();
  lines.clear();
  mocktoc.clear();

  foreach (SearchResult const &r, results) {
    TOCEntry *te = new TOCEntry;
    te->setStartPage(r.startPageOfEntry);
    te->setTitle(r.entryTitle);
    te->setCreated(r.created);
    te->setModified(r.modified);
    // set sheet count?
    mocktoc << te;
  }
  

  int sheet = 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = y0;

  for (int k=0; k<results.size(); k++) {
    QGraphicsItem *gi = new QGraphicsItem;
    addItem(gi);

    double h = gi->childrenBoundingRect().height();
    if (y+h > y1) {
      y = y0;
      sheet ++;
    }
    gi->setPos(QPointF(0, y));
    items << gi;
    sheetNos << sheet;
    lines << addLine(0, y, style().real("page-width"), y,
			 QPen(QBrush(style().color("toc-line-color")),
			      style().real("toc-line-width"))));
  }
}

QString SearchResultScene::title() 
  return ttl;
}
