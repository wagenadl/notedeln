
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


TextBlockItem::TextBlockItem(TextBlockData *data, Item *parent,
			     TICreator const &tic):
  BlockItem(data, parent) {
  item_ = 0;

  setPos(style().real("margin-left"), 0);

  item_ = tic.create(data->text(), this);

  initializeFormat();
  item_->setAllowParagraphs(false);
  item_->setAllowNotes(true);
  
  connect(item_, SIGNAL(textChanged()),
	  this, SLOT(sizeToFit()), Qt::QueuedConnection);
  // The non-instantaneous delivery is important, otherwise the check
  // may happen before the change is processed.
  connect(item_, SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	  this, SLOT(futileMovementKey(int, Qt::KeyboardModifiers)));
  connect(item_, SIGNAL(refTextChange(QString, QString)),
	  this, SLOT(refTextChange(QString, QString)));
}

void TextBlockItem::makeWritable() {
  BlockItem::makeWritable();
  item_->setAllowNotes(false);
  setFlag(ItemIsFocusable);
  setFocusProxy(item_);
}

TextBlockItem::~TextBlockItem() {
  // I assume the item_ is deleted by Qt?
}

void TextBlockItem::initializeFormat() {
  item_->setTextWidth(style().real("page-width")
		      - style().real("margin-left")
		      - style().real("margin-right"));
  item_->setPos(0, style().real("text-block-above"));

  QTextCursor tc(item_->document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setLineHeight(style().real("paragraph-line-spacing")*100,
		    QTextBlockFormat::ProportionalHeight);
  fmt.setTextIndent(data()->indented() ? style().real("paragraph-indent") : 0);
  //fmt.setTopMargin(style().real("paragraph-top-margin"));
  //  fmt.setBottomMargin(style().real("paragraph-bottom-margin"));
  tc.movePosition(QTextCursor::Start);
  tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  tc.setBlockFormat(fmt);
}  

void TextBlockItem::setFocus() {
  // do we need to check whether we want it?
  item_->setFocus();
}

QTextDocument *TextBlockItem::document() const {
  ASSERT(item_);
  return item_->document();
}

void TextBlockItem::futileMovementKey(int key, Qt::KeyboardModifiers mod) {
  fmi = FutileMovementInfo(key, mod, item_);
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

void TextBlockItem::acceptFocus(QPointF p) {
  qDebug() << "acceptFocus " << p;
  for (QTextBlock b = document()->firstBlock();
       b.length()>0;
       b = b.next()) {
    QTextLayout *lay = b.layout();
    qDebug() << " layout: " << lay->boundingRect() << " : " << lay->lineCount();
    if (lay->boundingRect().contains(p)) {
      QPointF dp = p - lay->position();
      int N = lay->lineCount();
      for (int n=0; n<N; n++) {
	QTextLine l(lay->lineAt(n));
	qDebug() << " line: " << l.rect();
	if (l.rect().contains(dp)) {
	  int pos = l.xToCursor(dp.x());
	  qDebug() << " gotcha: " << pos;
	  setFocus();
	  QTextCursor c(item_->textCursor());
	  c.setPosition(pos);
	  item_->setTextCursor(c);
	  return;
	}
      }
    }
  }
  qDebug() << " failed";
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
  return item_;
}

void TextBlockItem::sizeToFit() {
  QRectF r = item_->mapRectToParent(item_->netBounds());
  double h = data()->height();
  if (h!=r.height()) {
    if (isWritable())
      data()->setHeight(r.height());
    else
      data()->sneakilySetHeight(r.height());      
    emit heightChanged();
  }
}

  
