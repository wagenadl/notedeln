// SearchResultScene.cpp

#include "SearchResultScene.H"
#include "TOCEntry.H"
#include <QGraphicsItem>
#include "TOCItem.H"
#include <QTextDocument>
#include <QTextCursor>
#include "Roman.H"

SearchResultScene::SearchResultScene(QString title, QList<SearchResult> results,
                                     Data *data, QObject *parent):
  BaseScene(data, parent), ttl(title), results(results) {
  book = data->book();
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
  foreach (QGraphicsItem *i, items)
    delete i;
  foreach (QGraphicsItem *i, lines)
    delete i;
  foreach (TOCItem *i, headers)
    delete i;
  items.clear();
  itemsheetnos.clear();
  lines.clear();
  headers.clear();
  headersheetnos.clear();

  int oldPage = -1;

  int sheet = 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = y0;
  TOCItem *lastTOCItem = 0;
  foreach (SearchResult const &r, results) {
    if (r.startPageOfEntry != oldPage) {
      lines << addLine(0, 0, style().real("page-width"), 0,
                       QPen(QBrush(style().color("toc-line-color")),
                            style().real("toc-line-width")));
      lastTOCItem = new TOCItem(book->toc()->entry(r.startPageOfEntry), this);
      lines.last()->setParentItem(lastTOCItem);
      oldPage = r.startPageOfEntry;
      //connect(i, SIGNAL(vboxChanged()), SLOT(itemChanged()));
      connect(lastTOCItem, SIGNAL(clicked(int)),
              SIGNAL(pageNumberClicked(int)));
      if (y > y0 && y + lastTOCItem->childrenBoundingRect().height() > y1) {
        y = y0;
        sheet += 1;
      }
      lastTOCItem->setPos(0, y);
      headers << lastTOCItem;
      headersheetnos << sheet;
      y += lastTOCItem->childrenBoundingRect().height();
    }

    double h0 = lastTOCItem->childrenBoundingRect().bottom();
    QGraphicsTextItem *ti = new QGraphicsTextItem(lastTOCItem);
    ti->setFont(style().font("search-result-font"));
    QString clr;
    switch  (r.type) {
    case SearchResult::InTextBlock: clr = "text"; break;
    case SearchResult::InTableBlock: clr = "text"; break;
    case SearchResult::InGfxNote: clr = "note-text"; break;
    case SearchResult::InLateNote: clr = "latenote-text"; break;
    case SearchResult::InFootnote: clr = "footnote-def"; break;
    default: clr = "text"; break;
    }
    ti->setDefaultTextColor(style().color(clr + "-color"));
    fillText(ti->document(), r);
    ti->setPos(style().real("margin-left"), h0 - ti->boundingRect().top());
    ti->setTextWidth(style().real("page-width") 
                     - style().real("margin-left")
                     - style().real("margin-right"));
    y = lastTOCItem->pos().y() + lastTOCItem->childrenBoundingRect().height();
    lines.last()->setPos(0, lastTOCItem->childrenBoundingRect().height());
    if (lastTOCItem->pos().y() > y0 && y > y1) { // move to next sheet
      lastTOCItem->setPos(0, y0);
      headersheetnos.last() += 1;
      sheet += 1;
      y = y0 + lastTOCItem->childrenBoundingRect().height();
    }
  }
  nSheets = sheet+1;

  gotoSheet(0);
  
}

QString SearchResultScene::title() const { 
  return ttl;
}

int SearchResultScene::refineBreak(QString s, int idx) {
  if (idx<0)
    return idx;
  return s.indexOf(QRegExp("\\s"), idx);
}

int SearchResultScene::decentBreak(QString s, int first, int last) {
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

void SearchResultScene::fillText(QTextDocument *doc, SearchResult const &res) {
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
    c.insertText(res.phrase);
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

void SearchResultScene::gotoSheet(int sheet) {
  for (int n=0; n<headers.size(); n++) 
    if (headersheetnos[n] == sheet)
      headers[n]->show();
    else
      headers[n]->hide();

  for (int n=0; n<items.size(); n++) 
    if (itemsheetnos[n] == sheet)
      items[n]->show();
    else
      items[n]->hide();
  BaseScene::gotoSheet(sheet);
}

QString SearchResultScene::pgNoToString(int n) const {
  return Roman(n).lc();
}
  
void SearchResultScene::makeContdItems() {
  // Simply don't make them??
  BaseScene::makeContdItems();
  contdItem->setPos(4, style().real("margin-top"));
}
