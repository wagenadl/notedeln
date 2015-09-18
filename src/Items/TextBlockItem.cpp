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

#include "TextBlockItem.h"
#include "TextItem.h"
#include "TextItemDoc.h"
#include "TextBlockData.h"
#include "Style.h"
#include "EntryScene.h"

#include "TextCursor.h"
#include <QDebug>
#include "Assert.h"
#include <math.h>

TICreator::~TICreator() {
}

TextItem *TICreator::create(TextData *data, Item *parent,
			    TextItemDoc *altdoc) const {
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
  for (int i=0; i<ysplit.size(); i++) {
    TextItem *ti = tic.create(data->text(), 0, frags[0]->document());
    ti->setParentBlock(this);
    frags << ti;
  }

  ysplit.push_front(0);
  ysplit.push_back(data->height());
  QRectF r0 = frags[0]->netBounds();
  for (int i=0; i<frags.size(); i++) {
    if (frags.size()>1) 
      frags[i]->setClip(QRectF(r0.left(), ysplit[i],
			       r0.width(), ysplit[i+1]-ysplit[i]));
    connect(frags[i], SIGNAL(invisibleFocus(TextCursor, QPointF)),
	    SLOT(ensureVisible(TextCursor, QPointF)));
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
		   - style().real("margin-right"), false);

  ti->setFont(style().font(disp ? "display-text-font" : "text-font"));
  ti->setDefaultTextColor(style().color(disp ? "display-text-color"
					: "text-color"));
}  

void TextBlockItem::initializeFormat() {
  bool disp = data()->displayed();

  foreach (TextItem *ti, frags) 
    setTIFormat(ti);

  TextItemDoc *doc = frags[0]->document();
  TextCursor tc(doc);

  QString fontkey = disp ? "display-text-font" : "text-font";
  QString lskey = disp ? "display-paragraph-line-spacing"
    : "paragraph-line-spacing";
  double lh = style().lineSpacing(fontkey, lskey);
  double y0 = style().real("text-block-above");
  if (disp)
    y0 += .5*(lh - style().lineSpacing("text-font", "paragraph-line-spacing"));
  doc->setY0(y0);
  doc->setLineHeight(lh);
  
  double indent =
    data()->indented() ? style().real("paragraph-indent")
    : data()->dedented() ? -style().real("paragraph-indent")
    : 0;
  double leftmargin = disp ? style().real("display-paragraph-left-margin") : 4;
  if (data()->dedented())
    leftmargin +=  style().real("paragraph-indent");
  doc->setIndent(indent);
  doc->setLeftMargin(leftmargin);
  doc->setRightMargin(disp ? style().real("display-paragraph-right-margin")
                      : 0);
  if (data()->text()->lineStarts().isEmpty())
    doc->relayout();
  else
    doc->buildLinePos();
}  

TextItemDoc *TextBlockItem::document() const {
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
  TextData *dat = text()->data();
  QList<int> pargs = dat->paragraphStarts();
  return pargs.last()==dat->text().size();
}

void TextBlockItem::dropEmptyLastPar() {
  if (lastParIsEmpty()) {
    TextCursor c(document());
    c.movePosition(TextCursor::End);
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
  frags[0]->document()->relayout(); // is this needed?
  double h0 = data()->height();
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

TextCursor TextBlockItem::textCursor() const {
  foreach (TextItem *ti, frags)
    if (ti->hasFocus())
      return ti->textCursor();
  return text()->textCursor();
}

void TextBlockItem::setTextCursor(TextCursor c) {
  QPointF pos = frags[0]->posToPoint(c.position());
  int tgt = 0;
  for (int i=0; i<frags.size(); i++) {
    if (!frags[i]->clips()
	|| frags[i]->clipRect().contains(pos)) {
      tgt = i;
      break;
    }
  }
  frags[tgt]->setFocus();
  frags[tgt]->setTextCursor(c);
  emit sheetRequest(data()->sheet() + tgt);
  frags[tgt]->setFocus();
}

void TextBlockItem::ensureVisible(TextCursor c, QPointF p) {
  qDebug() << "ensureVisible" << data()->text()->text() << c.position() << p;
  for (int i=0; i<frags.size(); i++) {
    if (!frags[i]->clips() || frags[i]->clipRect().contains(p)) {
      qDebug() << "  emitting sheetRequest" << data()->sheet() + i;
      emit sheetRequest(data()->sheet() + i);
      frags[i]->setFocus();
      TextCursor c1(frags[i]->textCursor());
      if (c.hasSelection()) {
        c1.setPosition(c.anchor());
        c1.setPosition(c.position(), TextCursor::KeepAnchor);
      } else {
        c1.setPosition(c.position());
      }
      frags[i]->setTextCursor(c1);
      return;
    }
  }
}

double TextBlockItem::visibleHeight() const {
  return frags[0]->document()->visibleHeight();
}

double TextBlockItem::splittableY(double y) const {
  if (y >= data()->height())
    return data()->height();
  else
    return frags[0]->document()->splittableY(y);
}

void TextBlockItem::unsplit() {
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
    connect(ti, SIGNAL(invisibleFocus(TextCursor, QPointF)),
	    SLOT(ensureVisible(TextCursor, QPointF)));
    ti->setParentBlock(this);
    
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
  BlockItem::mousePressEvent(e);
}

void TextBlockItem::focusInEvent(QFocusEvent*e) {
  BlockItem::focusInEvent(e);
}

int TextBlockItem::findFragmentForPhrase(QString phrase) const {
  int k = document()->find(phrase);
  if (k<0)
    return -1;
  double y = document()->locate(k).y();
  int i=0;
  foreach (int y0, data()->sheetSplits()) {
    if (y<y0)
      return i;
    i++;
  }
  return i;
}

void TextBlockItem::muckWithIndentation(TextCursor c) {
  initializeFormat();
  document()->relayout();
  sizeToFit();
  QPointF p = frags[0]->posToPoint(c.position());
  ensureVisible(c, p);
}
