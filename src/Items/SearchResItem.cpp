// Items/SearchResItem.cpp - This file is part of NotedELN

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

// SearchResItem.cpp

#include "SearchResItem.h"
#include "Search.h"
#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QTextCursor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

SearchResItem::SearchResItem(TOCEntry *data, BaseScene *parent):
  TOCItem(data, parent) {
}

SearchResItem::~SearchResItem() {
}

void SearchResItem::reset() {
  foreach (QGraphicsTextItem *i, items)
    delete i;
  items.clear();
  ytop.clear();
  pgno.clear();
  uuid.clear();
}

void SearchResItem::addResult(SearchResult const &res,
                              QGraphicsItem *parent) {
  ASSERT(parent);
  double h0 = parent->childrenBoundingRect().bottom();
  QGraphicsTextItem *ti = new QGraphicsTextItem(parent);
  ti->setFont(style().font("search-result-font"));
  QString clr;
  switch  (res.type) {
  case SearchResult::InTextBlock: clr = "text"; break;
  case SearchResult::InTableBlock: clr = "text"; break;
  case SearchResult::InGfxNote: clr = "note-text"; break;
  case SearchResult::InLateNote: clr = "latenote-text"; break;
  case SearchResult::InFootnote: clr = "footnote-def"; break;
  default: clr = "text"; break;
  }
  ti->setDefaultTextColor(style().color(clr + "-color"));
  fillText(ti->document(), res);
  ti->setPos(style().real("margin-left"), h0 - ti->boundingRect().top());
  ti->setTextWidth(style().real("page-width") 
                   - style().real("margin-left")
                   - style().real("margin-right"));
  items << ti;
  ytop << h0;
  pgno << res.page;
  uuid << res.uuid;
}


int SearchResItem::refineBreak(QString s, int idx) {
  if (idx<0)
    return idx;
  return s.indexOf(QRegExp("\\s"), idx);
}

int SearchResItem::decentBreak(QString s, int first, int last) {
  if (first<0)
    first = 0;
  int idx = s.indexOf(".", first);
  idx = refineBreak(s, idx);
  if (idx>=0 && idx<last) 
    return idx;

  idx = s.indexOf(QRegExp("[!?][^)]"), first);
  idx = refineBreak(s, idx);
  if (idx>=0 && idx<last) 
    return idx;

  idx = s.indexOf(QRegExp("[!?,;]"), first);
  idx = refineBreak(s, idx);
  if (idx>=0 && idx<last) 
    return idx;

  idx = s.indexOf(QRegExp(QString::fromUtf8("[])”’]")), first);
  idx = refineBreak(s, idx);
  if (idx>=0 && idx<last) 
    return idx;

  idx = s.indexOf(QRegExp("\\s"), first);
  if (idx>=0 && idx<last) 
    return idx;

  return -1;
}

void SearchResItem::fillText(QTextDocument *doc, SearchResult const &res) {
  doc->clear();
  bool nextpredone = false;
  for (int k=0; k<res.whereInContext.size(); k++) {
    int strt = res.whereInContext[k];
    int end = strt + res.phrase.size();
    if (!nextpredone) {
      int prebreak = decentBreak(res.context, strt - 80, strt - 40);
      if (prebreak<0)
        prebreak = 0;
      QTextCursor c(doc);
      c.movePosition(QTextCursor::End);
      if (prebreak>0)
        c.insertText(QString::fromUtf8("…"));
      c.insertText(res.context.mid(prebreak, strt-prebreak));
    }

    QTextCursor c(doc);
    c.movePosition(QTextCursor::End);
    int m0 = c.position();
    //    c.insertText(res.phrase);
    c.insertText(res.context.mid(strt, end-strt));
    int m1 = c.position();

    int postbreak = (k<res.whereInContext.size()-1)
      ? decentBreak(res.context, end + 80, end + 120)
      : decentBreak(res.context, end + 40, end + 80);
    if (postbreak<0) {
      postbreak = (k<res.whereInContext.size()-1)
        ? res.whereInContext[k+1]
        : res.context.size();
      nextpredone = true;     
    } else {
      nextpredone = false;
    }

    c.movePosition(QTextCursor::End);
    c.insertText(res.context.mid(end, postbreak-end));

    c.setPosition(m0);
    c.setPosition(m1, QTextCursor::KeepAnchor);
    QTextCharFormat f = c.charFormat();
    f.setBackground(style().color("search-result-background-color"));
    c.setCharFormat(f);
  }
  if (!nextpredone) {
    QTextCursor c(doc);
    c.movePosition(QTextCursor::End);
    c.insertText(QString::fromUtf8(" …"));
  }
}

void SearchResItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  int pg = -1;
  QString id;
  for (int k=0; k<items.size(); k++) {
    if (e->pos().y() >= ytop[k]) {
      pg = pgno[k];
      id = uuid[k];
    }
  }
  if (pg>=0) {
    emit clicked(pg, e->modifiers(), id);
  } else {
    TOCItem::mousePressEvent(e);
  }
}
