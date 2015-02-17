// Items/TextBlockItem.cpp - This file is part of eln

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

// TextBlockItem.C

#include "TextBlockItem.H"
#include "TextItem.H"
#include "TextItemText.H"
#include "TextBlockData.H"
#include "Style.H"
#include "EntryScene.H"

#include <QTextCursor>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextDocument>
#include <QDebug>
#include "Assert.H"

TICreator::~TICreator() {
}

TextItem *TICreator::create(TextData *data, Item *parent,
			    QTextDocument *altdoc) const {
  return new TextItem(data, parent, false, altdoc);
}

/* This TICreator business is so that TableBlockItem's TableItem can
   be created by TextBlockItem without trouble.
 */


TextBlockItem::TextBlockItem(TextBlockData *data, Item *parent,
			     TICreator const &tic):
  BlockItem(data, parent), tic(tic) {

  setPos(style().real("margin-left"), 0);

  frags << tic.create(data->text(), this);
  QList<double> ysplit = data->sheetSplits();
  for (int i=0; i<ysplit.size(); i++) 
    frags << tic.create(data->text(), 0, frags[0]->document());

  ysplit.push_front(0);
  ysplit.push_back(data->height());
  QRectF r0 = frags[0]->netBounds();
  for (int i=0; i<frags.size(); i++) {
    if (frags.size()>1) 
      frags[i]->setClip(QRectF(r0.left(), ysplit[i],
			       r0.width(), ysplit[i+1]-ysplit[i]));
    connect(frags[i], SIGNAL(invisibleFocus(QPointF)),
	    SLOT(ensureVisible(QPointF)));
    connect(frags[i], SIGNAL(multicellular(int, TextData*)),
	    this, SIGNAL(multicellular(int, TextData*)));

  }
  
  initializeFormat();

  foreach (TextItem *ti, frags) {
    ti->setAllowParagraphs(false);
    ti->setAllowNotes(true);
  }

  foreach (TextItem *ti, frags) {
    connect(ti, SIGNAL(textChanged()),
	    this, SLOT(sizeToFit()), Qt::QueuedConnection);
    // The non-instantaneous delivery is important, otherwise the check
    // may happen before the change is processed.
    connect(ti, SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	    this, SLOT(futileMovementKey(int, Qt::KeyboardModifiers)));
    connect(ti, SIGNAL(refTextChange(QString, QString)),
	    this, SLOT(refTextChange(QString, QString)));
  }
}

void TextBlockItem::makeWritable() {
  BlockItem::makeWritable(); // this makes the first fragment writable
  for (int i=1; i<frags.size(); i++)
    frags[i]->makeWritable();
  setFlag(ItemIsFocusable);
  setFocusProxy(frags[0]);
}

TextBlockItem::~TextBlockItem() {
  // The frags[0] is deleted by Qt
}

void TextBlockItem::setTIFormat(TextItem *ti) {
  bool disp = data()->displayed();

  ti->setTextWidth(style().real("page-width")
		   - style().real("margin-left")
		   - style().real("margin-right"));
  ti->setPos(0, style().real("text-block-above"));

  ti->setFont(style().font(disp ? "display-text-font" : "text-font"));
  ti->setDefaultTextColor(style().color(disp ? "display-text-color"
					: "text-color"));
}  

void TextBlockItem::initializeFormat() {
  bool disp = data()->displayed();

  foreach (TextItem *ti, frags) 
    setTIFormat(ti);

  QTextCursor tc(frags[0]->document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setLineHeight(style().real(disp ? "display-paragraph-line-spacing"
                                 : "paragraph-line-spacing")*100,
                    QTextBlockFormat::ProportionalHeight);
  double indent =
    data()->indented() ? style().real("paragraph-indent")
    : data()->dedented() ? -style().real("paragraph-indent")
    : 0;
  double leftmargin = disp ? style().real("display-paragraph-left-margin") : 0;
  if (data()->dedented())
    leftmargin +=  style().real("paragraph-indent");
  fmt.setTextIndent(indent);
  fmt.setLeftMargin(leftmargin);
  fmt.setRightMargin(disp ? style().real("display-paragraph-right-margin") : 0);
  //fmt.setTopMargin(style().real("paragraph-top-margin"));
  //  fmt.setBottomMargin(style().real("paragraph-bottom-margin"));
  tc.movePosition(QTextCursor::Start);
  tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  tc.setBlockFormat(fmt);
}  

QTextDocument *TextBlockItem::document() const {
  ASSERT(frags[0]);
  return frags[0]->document();
}

void TextBlockItem::futileMovementKey(int key, Qt::KeyboardModifiers mod) {
  fmi = FutileMovementInfo(key, mod, frags[0]);
  emit futileMovement(); // we emit w/o content, because EntryScene uses Mapper.
}

bool TextBlockItem::isEmpty() const {
  return document()->isEmpty();
}

bool TextBlockItem::lastParIsEmpty() const {
  QTextBlock b = document()->lastBlock();
  QString t = b.text();
  bool e = t.isEmpty();
  return e;
}

void TextBlockItem::dropEmptyLastPar() {
  if (lastParIsEmpty()) {
    QTextCursor c(document());
    c.movePosition(QTextCursor::End);
    c.deletePreviousChar();
  }
}

FutileMovementInfo const &TextBlockItem::lastFutileMovement() const {
  return fmi;
}

QRectF TextBlockItem::boundingRect() const {
  return QRectF();
  // text draws itself
}

void TextBlockItem::paint(QPainter *,
		      const QStyleOptionGraphicsItem *,
		      QWidget *) {
  // text draws itself
}

TextItem *TextBlockItem::text() const {
  return frags[0];
}

void TextBlockItem::sizeToFit() {
  if (beingDeleted())
    return;
  double h0 = data()->height();
  //  double h1 = 0;
  //  foreach (TextItem *ti, frags) 
  //    h1 += ti->mapRectToParent(ti->netBounds()).height();
  double h1 = frags[0]->mapRectToParent(frags[0]->netBounds()).height();
  if (h1!=h0) {
    if (isWritable())
      data()->setHeight(h1);
    else
      data()->sneakilySetHeight(h1);
    emit heightChanged();
  }
}

class TextItem *TextBlockItem::fragment(int fragno) {
  ASSERT(fragno>=0 && fragno<frags.size());
  return frags[fragno];
}

int TextBlockItem::nFragments() const {
  return frags.size();
}

QList<TextItem *> TextBlockItem::fragments() {
  QList<TextItem *> l;
  foreach (TextItem *f, frags)
    l << f;
  return l;
}

QTextCursor TextBlockItem::textCursor() const {
  foreach (TextItem *ti, frags)
    if (ti->hasFocus())
      return ti->textCursor();
  return text()->textCursor();
}

void TextBlockItem::setTextCursor(QTextCursor c) {
  QPointF pos = frags[0]->posToPoint(c.position());
  int tgt = 0;
  qDebug() << "TBI::setTextCursor" << pos;
  for (int i=0; i<frags.size(); i++) {
    if (!frags[i]->clips()
	|| frags[i]->clipRect().contains(pos)) {
      qDebug() << "   gotcha " << i;
      tgt = i;
      break;
    }
  }
  frags[tgt]->setFocus();
  frags[tgt]->setTextCursor(c);
  emit sheetRequest(data()->sheet() + tgt);
  frags[tgt]->setFocus();
}

void TextBlockItem::ensureVisible(QPointF p) {
  qDebug() << "TBI: ensure visible";
  for (int i=0; i<frags.size(); i++) {
    if (frags[i]->clipRect().contains(p)) {
      qDebug() << "TBI: ensure visible scene" << i;
      emit sheetRequest(data()->sheet() + i);
      frags[i]->setFocus();
      int pos = frags[i]->pointToPos(p);
      QTextCursor c(frags[i]->textCursor());
      c.setPosition(pos);
      frags[i]->setTextCursor(c);
      return;
    }
  }
}

double TextBlockItem::splittableY(double y) {
  QTextDocument *doc = frags[0]->document();
  double bestY = 0;
  qDebug() << "TBI: h " << data()->height() << netBounds().height()
	   << text()->netBounds().height()
	   << text()->titxt()->boundingRect().height();
  for (QTextBlock blk = doc->firstBlock(); blk.isValid(); blk=blk.next()) {
    QTextLayout *lay = blk.layout();
    double y0 = lay->position().y();
    QRectF bb = lay->boundingRect();
    qDebug() << "TBI: sply " << y << y0 << bb;
    for (int i=0; i<lay->lineCount(); i++) {
      qDebug() << "  " << i << lay->lineAt(i).rect().bottom();
    }
    if (y0 + bb.bottom() <= y) {
      bestY = y0 + bb.bottom();
    } else if (y0+bb.top()<y) {
      for (int i=0; i<lay->lineCount(); i++) {
	QTextLine ln = lay->lineAt(i);
	if (y0 + ln.rect().bottom() <= y)
	  bestY = y0 + ln.rect().bottom();
      }
    }
  }
  return bestY;
}

void TextBlockItem::unsplit() {
  qDebug() << "TBI:unsplit" << frags.size() << data()->sheetSplits().size();
  while (frags.size()>1) {
    TextItem *ti = frags.takeLast();
    if (ti)
      ti->deleteLater();
    else
      qDebug() << "null item??";
  }
  frags[0]->unclip();
}

void TextBlockItem::split(QList<double> ysplit) {
  if (ysplit.isEmpty()) {
    unsplit();
    return;
  }
  
  while (frags.size()>1+ysplit.size()) {
    TextItem *ti = frags.takeLast();
    if (ti)
      ti->deleteLater();
    else
      qDebug() << "TBI:Split: null item??";
  }

  while (frags.size()<1+ysplit.size()) {
    TextItem *ti = tic.create(data()->text(), 0, frags[0]->document());
    connect(ti, SIGNAL(invisibleFocus(QPointF)),
	    SLOT(ensureVisible(QPointF)));
    frags << ti;

    if (frags[0]->isWritable())
      ti->makeWritable();
    setTIFormat(ti);
    ti->setAllowParagraphs(false);
    ti->setAllowNotes(true);

    connect(ti, SIGNAL(textChanged()),
	    this, SLOT(sizeToFit()), Qt::QueuedConnection);
    // The non-instantaneous delivery is important, otherwise the check
    // may happen before the change is processed.
    connect(ti, SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	    this, SLOT(futileMovementKey(int, Qt::KeyboardModifiers)));
    connect(ti, SIGNAL(refTextChange(QString, QString)),
	    this, SLOT(refTextChange(QString, QString)));

  }

  ysplit.push_front(0);
  ysplit.push_back(data()->height());
  QRectF r0 = frags[0]->netBounds();
  for (int i=0; i<frags.size(); i++)
    frags[i]->setClip(QRectF(r0.left(), ysplit[i],
			     r0.width(), ysplit[i+1]-ysplit[i]));
}

void TextBlockItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "TBI: mousepress";
  BlockItem::mousePressEvent(e);
}

void TextBlockItem::focusInEvent(QFocusEvent*e) {
  qDebug() << "TBI: focusin";
  BlockItem::focusInEvent(e);
}

int TextBlockItem::findFragmentForPhrase(QString phrase) const {
  QTextCursor c(document()->find(phrase));
  if (c.isNull())
    return -1;
  QTextBlock b = document()->findBlock(c.position());
  if (!b.isValid())
    return -1;
  QTextLayout *lay = b.layout();
  QTextLine l = lay->lineForTextPosition(c.position()-b.position());
  int y = l.position().y();
  int i=0;
  foreach (int y0, data()->sheetSplits()) {
    if (y<y0)
      return i;
    i++;
  }
  return i;
}
