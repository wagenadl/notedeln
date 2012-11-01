// FootnoteItem.C

#include "FootnoteItem.H"
#include "FootnoteData.H"
#include "TextItem.H"

FootnoteItem::FootnoteItem(FootnoteData *data, Item *parent):
  QGraphicsItem(Item::gi(parent)),
  Item(data, this),
  data_(data) {
  Q_ASSERT(data);
  Q_ASSERT(data->book());
  tag = new QGraphicsTextItem(this);
  tag->setPlainText(data->tag());
  def = new TextItem(data, this);
  addChild(def);

  tag->setFont(QFont(style().string("footnote-tag-font-family"),
		     style().real("footnote-tag-font-size")));
  tag->setDefaultTextColor(style().color("footnote-tag-color"));

  def->setFont(QFont(style().string("footnote-def-font-family"),
		     style().real("footnote-def-font-size")));
  def->setDefaultTextColor(style().color("footnote-def-color"));
  
  double textwidth = style().real("page-width")
    - style().real("margin-left")
    - style().real("margin-right");
  double tagwidth = tag->boundingRect().width();
  def->setPos(tagwidth, 0);
  def->setTextWidth(textwidth - tagwidth);
}

FootnoteItem::~FootnoteItem() {
}

FootnoteData *FootnoteItem::data() {
  return data_;
}

QGraphicsTextItem *FootnoteItem::tagItem() {
  return tag;
}

TextItem *FootnoteItem::defItem() {
  return def;
}

void FootnoteItem::makeWritable() {
  def->makeWritable();
}

QRectF FootnoteItem::boundingRect() const {
  return QRectF();
}

void FootnoteItem::paint(QPainter *, const QStyleOptionGraphicsItem *,
			 QWidget *) {
}

