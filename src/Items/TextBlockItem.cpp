
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

TextItem *TICreator::create(TextData *data, Item *parent) const {
  return new TextItem(data, parent);
}

/* This TICreator business is so that TableBlockItem's TableItem can
   be created by TextBlockItem without trouble.
 */


TextBlockItem::TextBlockItem(TextBlockData *data, Item *parent,
			     TICreator const &tic):
  BlockItem(data, parent) {

  setPos(style().real("margin-left"), 0);

  frags << tic.create(data->text(), this);

  initializeFormat();
  frags[0]->setAllowParagraphs(false);
 frags[0]->setAllowNotes(true);
  
  connect(frags[0], SIGNAL(textChanged()),
	  this, SLOT(sizeToFit()), Qt::QueuedConnection);
  // The non-instantaneous delivery is important, otherwise the check
  // may happen before the change is processed.
  connect(frags[0], SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	  this, SLOT(futileMovementKey(int, Qt::KeyboardModifiers)));
  connect(frags[0], SIGNAL(refTextChange(QString, QString)),
	  this, SLOT(refTextChange(QString, QString)));
}

void TextBlockItem::makeWritable() {
  BlockItem::makeWritable();
  setFlag(ItemIsFocusable);
  setFocusProxy(frags[0]);
}

TextBlockItem::~TextBlockItem() {
  // The frags[0] is deleted by Qt
}

void TextBlockItem::initializeFormat() {
  bool disp = data()->displayed();

  foreach (TextItem *ti, frags) {
    ti->setTextWidth(style().real("page-width")
                     - style().real("margin-left")
                     - style().real("margin-right"));
    ti->setPos(0, style().real("text-block-above"));

    ti->setFont(style().font(disp ? "display-text-font" : "text-font"));
    ti->setDefaultTextColor(style().color(disp ? "display-text-color"
                                                : "text-color"));
  }

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
  double h0 = data()->height();
  double h1 = 0;
  foreach (TextItem *ti, frags) 
    h1 += ti->mapRectToParent(ti->netBounds()).height();
  if (h1!=h0) {
    if (isWritable())
      data()->setHeight(h1);
    else
      data()->sneakilySetHeight(h1);
    emit heightChanged();
  }
}

class TextItem *TextBlockItem::fragment(int fragno) const {
  ASSERT(fragno>=0 && fragno<frags.size());
  return frags[fragno];
}

int TextBlockItem::nFragments() const {
  return frags.size();
}

int TextBlockItem::fragmentForPos(int pos) const {
  if (!frags[0]->clips())
    return 0;
  for (int i=0; i<frags.size(); i++) {
    QPointF xy = frags[i]->posToPoint(pos);
    if (frags[i]->clipPath().boundingRect().contains(xy))
      return i;
  }
  qDebug() << "TBI::fragmentForPos: not found, returning 0";
  return 0;
}

void TextBlockItem::unsplit() {
  while (frags.size()>1)
    frags.takeLast()->deleteLater();
  frags[0]->unclip();
}

void TextBlockItem::splitAt(double yoffset) {
}
