// SearchResultScene.cpp

#include "SearchResultScene.h"
#include "TOCEntry.h"
#include <QGraphicsItem>
#include "SearchResItem.h"
#include <QTextDocument>
#include <QTextCursor>
#include "Roman.h"
#include <QDebug>
#include "SheetScene.h"

SearchResultScene::SearchResultScene(QString phrase, QString title,
				     QList<SearchResult> results,
                                     Data *data, QObject *parent):
  BaseScene(data, parent), phrase(phrase), ttl(title), results(results) {
  book = data->book();
  setContInMargin();
}

SearchResultScene::~SearchResultScene() {
}

Style const &SearchResultScene::style() const {
  return book->style();
}

void SearchResultScene::update(QList<SearchResult> res) {
  results = res;
  populate();
}

void SearchResultScene::populate() {
  BaseScene::populate();
  foreach (TOCItem *i, headers)
    delete i;
  headers.clear();
  sheetnos.clear();

  int oldPage = -1;

  int sheet = 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = y0;
  QGraphicsLineItem *lastLine = 0;
  foreach (SearchResult const &r, results) {
    if (r.startPageOfEntry != oldPage) {
      lastLine = new QGraphicsLineItem(0, 0, style().real("page-width"), 0);
      lastLine->setPen(QPen(QBrush(style().color("toc-line-color")),
			    style().real("toc-line-width")));
      headers << new SearchResItem(book->toc()->tocEntry(r.startPageOfEntry),
                                   this);
      lastLine->setParentItem(headers.last());
      oldPage = r.startPageOfEntry;
      connect(headers.last(),
              SIGNAL(clicked(int, Qt::KeyboardModifiers, QString)),
              SLOT(pageNumberClick(int, Qt::KeyboardModifiers, QString)));
      if (y > y0 && y + headers.last()->childrenBoundingRect().height() > y1) {
        y = y0;
        sheet += 1;
      }
      headers.last()->setPos(0, y);
      this->sheet(sheet,true)->addItem(headers.last());
      sheetnos << sheet;
      y += headers.last()->childrenBoundingRect().height();
    }

    headers.last()->addResult(r, headers.last());
    y = headers.last()->pos().y()
      + headers.last()->childrenBoundingRect().height();
    lastLine->setPos(0, headers.last()->childrenBoundingRect().height());
    if (headers.last()->pos().y() > y0 && y > y1) { // move to next sheet
      headers.last()->setPos(0, y0);
      sheetnos.last() += 1;
      sheet += 1;
      this->sheet(sheet,true)->addItem(headers.last());
      y = y0 + headers.last()->childrenBoundingRect().height();
    }
  }
  nSheets = sheet+1;
}

QString SearchResultScene::title() const { 
  return ttl;
}

QString SearchResultScene::pgNoToString(int n) const {
  return Roman(n).lc();
}
  
void SearchResultScene::pageNumberClick(int pg, Qt::KeyboardModifiers m,
                                        QString id) {
  emit pageNumberClicked(pg, m, id, phrase);
}

  
