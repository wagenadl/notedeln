// FootnoteItem.C

#include "FootnoteItem.H"
#include "FootnoteData.H"
#include "TextItem.H"

FootnoteItem::FootnoteItem(FootnoteData *data, Item *parent):
  TextBlockItem(data, parent), data_(data) {
  Q_ASSERT(data);
  Q_ASSERT(data->book());
  tag_ = new QGraphicsTextItem(this);
  tag_->setPlainText(data->tag() + ":");

  tag_->setFont(QFont(style().string("footnote-tag-font-family"),
		     style().real("footnote-tag-font-size")));
  tag_->setDefaultTextColor(style().color("footnote-tag-color"));

  text()->setFont(QFont(style().string("footnote-def-font-family"),
		     style().real("footnote-def-font-size")));
  text()->setDefaultTextColor(style().color("footnote-def-color"));
  
  double textwidth = style().real("page-width")
    - style().real("margin-left")
    - style().real("margin-right");
  double tagwidth = tag_->boundingRect().width();
  text()->setPos(tagwidth, 0);
  text()->setTextWidth(textwidth - tagwidth);
  text()->setAllowParagraphs(false);

  QTextCursor tc(text()->document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setLineHeight(100, QTextBlockFormat::ProportionalHeight);
  fmt.setTextIndent(0.0);
  fmt.setTopMargin(0.0);
  fmt.setBottomMargin(0.0);
  tc.setBlockFormat(fmt);

}

FootnoteItem::~FootnoteItem() {
}

FootnoteData *FootnoteItem::data() {
  return data_;
}

QGraphicsTextItem *FootnoteItem::tag() {
  return tag_;
}
