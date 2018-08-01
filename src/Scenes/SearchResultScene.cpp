// Scenes/SearchResultScene.cpp - This file is part of eln

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
  allInstances().insert(this);
  book = data->book();
  setContInMargin();
}

SearchResultScene::~SearchResultScene() {
  allInstances().remove(this);
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
	createContinuationItem(sheet, y, y1);
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
      createContinuationItem(sheet, headers.last()->pos().y(), y1);
      headers.last()->setPos(0, y0);
      sheetnos.last() += 1;
      sheet += 1;
      this->sheet(sheet,true)->addItem(headers.last());
      y = y0 + headers.last()->childrenBoundingRect().height();
    }
  }
  nSheets = sheet+1;
}

void SearchResultScene::createContinuationItem(int i, double ytop, double ybot) {
  QGraphicsTextItem *item = new QGraphicsTextItem(style().string("continued"));
  QFont f = style().font("title-font");
  f.setStyle(QFont::StyleItalic);
  item->setFont(f);
  item->setDefaultTextColor(style().color("latenote-text-color"));
  this->sheet(i, true)->addItem(item);
  double x0 = style().real("margin-left");
  double w0 = style().real("page-width") - style().real("margin-right")
    - style().real("margin-left");
  double h0 = ybot - ytop;
  QSizeF wh = item->boundingRect().size();
  double w = wh.width();
  double h = wh.height();
  double x = x0 + (w0 - w) / 2;
  double y = ytop + (h0 - h) / 2;
  double y0 = ytop + 12; // enforce a little margin
  if (y < y0)
    y = y0;
  item->setPos(x, y);
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

QSet<SearchResultScene*> &SearchResultScene::allInstances() {
  static QSet<SearchResultScene *> set;
  return set;
}

QMap< QString, QList<SearchResult> > SearchResultScene::allOpenSearches() {
  QMap< QString, QList<SearchResult> > res;
  for (SearchResultScene const *srs: allInstances())
    res[srs->phrase] = srs->results;
  return res;
}
