// TextBlockItem.C

#include "TextBlockItem.H"
#include "TextItem.H"
#include "TextBlockData.H"
#include "Style.H"
#include "PageScene.H"

#include <QTextCursor>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextDocument>
#include <QDebug>
#include "Assert.H"

TextBlockItem::TextBlockItem(TextBlockData *data, Item *parent):
  BlockItem(data, parent) {
  item_ = 0;

  setPos(style().real("margin-left"), 0);

  item_ = new TextItem(data->text(), this);

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
  fmt.setTextIndent(style().real("paragraph-indent"));
  //fmt.setTopMargin(style().real("paragraph-top-margin"));
  //  fmt.setBottomMargin(style().real("paragraph-bottom-margin"));
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

TextItem *TextBlockItem::text() {
  return item_;
}

void TextBlockItem::sizeToFit() {
  QRectF r = item_->mapRectToParent(item_->netBounds());
  double h = data()->height();
  if (h!=r.height()) {
    data()->setHeight(r.height());
    emit heightChanged();
  }
}

  
