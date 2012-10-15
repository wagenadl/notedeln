// TextBlockItem.C

#include "TextBlockItem.H"
#include "TextItem.H"
#include "TextBlockData.H"
#include "Style.H"

#include <QTextCursor>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextDocument>
#include <QDebug>

TextBlockItem::TextBlockItem(TextBlockData *data, PageScene *parent):
  BlockItem(data, parent),
  data_(data) {
  item_ = 0;

  setPos(Style::defaultStyle()["margin-left"].toDouble(), 0);

  item_ = new TextItem(data_->text(), this);

  initializeFormat();
  item_->setAllowParagraphs(false);
  
  connect(item_, SIGNAL(textChanged()),
	  this, SLOT(checkVbox()));
  connect(item_, SIGNAL(futileMovementKey(QTextCursor,
					  int, Qt::KeyboardModifiers)),
	  this, SLOT(futileMovementKey(QTextCursor,
				       int, Qt::KeyboardModifiers)));
  connect(item_, SIGNAL(enterPressed()),
	  this, SIGNAL(enterPressed()));

  resetBbox();
}

TextBlockItem::~TextBlockItem() {
  // I assume the item_ is deleted by Qt?
}

void TextBlockItem::initializeFormat() {
  Style const &style(Style::defaultStyle());
  item_->setTextWidth(style["paragraph-width"].toDouble());
  //item_->setFont(QFont(style["text-font-family"].toString(),
  //style["text-font-size"].toDouble()));
  //item_->setDefaultTextColor(QColor(style["text-color"].toString()));

  QTextCursor tc(item_->document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setLineHeight(style["paragraph-line-spacing"].toDouble()*100,
		    QTextBlockFormat::ProportionalHeight);
  fmt.setTextIndent(style["paragraph-indent"].toDouble());
  fmt.setTopMargin(style["paragraph-top-margin"].toDouble());
  fmt.setBottomMargin(style["paragraph-bottom-margin"].toDouble());
  tc.setBlockFormat(fmt);
}  

TextBlockData *TextBlockItem::data() {
  return data_;
}

void TextBlockItem::setFocus() {
  // do we need to check whether we want it?
  item_->setFocus();
}

QTextDocument *TextBlockItem::document() const {
  Q_ASSERT(item_);
  return item_->document();
}

void TextBlockItem::futileMovementKey(QTextCursor c,
				      int key,
				      Qt::KeyboardModifiers mod) {
  Q_ASSERT(item_);
  QTextBlock b = c.block();
  QTextLayout *lay = b.layout();
  QPointF xy0 = lay->position(); // in item
  int p = c.positionInBlock();
  QTextLine line = lay->lineForTextPosition(p);
  QPointF xy(line.cursorToX(p), line.y()+line.ascent()); // in layout

  fmi.pos_ = item_->mapToParent(xy0 + xy);
  fmi.scenePos_ = mapToParent(fmi.pos_);
  fmi.key_ = key;
  fmi.modifiers_ = mod;
  emit futileMovement(); // we emit w/o content, because PageScene uses Mapper.
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

TextBlockItem::FutileMovementInfo const &TextBlockItem::lastFutileMovement()
  const {
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

QRectF TextBlockItem::netBoundingRect() const {
  return item_
    ? item_->boundingRect()
    : QRectF();
}

void TextBlockItem::paint(QPainter *,
		      const QStyleOptionGraphicsItem *,
		      QWidget *) {
  // text draws itself
}

TextItem *TextBlockItem::text() {
  return item_;
}

QPointF TextBlockItem::FutileMovementInfo::pos() const {
  return pos_;
}

QPointF TextBlockItem::FutileMovementInfo::scenePos() const {
  return scenePos_;
}

int TextBlockItem::FutileMovementInfo::key() const {
  return key_;
}

Qt::KeyboardModifiers TextBlockItem::FutileMovementInfo::modifiers() const {
  return modifiers_;
}

