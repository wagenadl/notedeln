// FootnoteItem.C

#include "FootnoteItem.H"
#include "FootnoteData.H"
#include "TextItem.H"
#include "FootnoteGroupItem.H"
#include "AutoNote.H"
#include "Assert.H"

#include <QDebug>

FootnoteItem::FootnoteItem(FootnoteData *data, Item *parent):
  TextBlockItem(data, parent) {
  ASSERT(data->book());
  tag_ = new QGraphicsTextItem(this);

  tag_->setFont(style().font("footnote-tag-font"));
  tag_->setDefaultTextColor(style().color("footnote-tag-color"));

  text()->setFont(style().font("footnote-def-font"));
  text()->setDefaultTextColor(style().color("footnote-def-color"));

  updateTag();
  text()->setAllowParagraphs(false);

  QTextCursor tc(text()->document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setLineHeight(100, QTextBlockFormat::ProportionalHeight);
  fmt.setTextIndent(0.0);
  fmt.setTopMargin(0.0);
  fmt.setBottomMargin(0.0);
  tc.setBlockFormat(fmt);
  text()->setTextWidth(text()->textWidth()); // crazy way to fix HoverRegions
  connect(text(), SIGNAL(abandoned()), this, SLOT(abandon()));

  FootnoteGroupItem *fng = dynamic_cast<FootnoteGroupItem*>(parent);
  if (fng) {
    connect(this, SIGNAL(heightChanged()), fng, SLOT(childChanged()));
    connect(this, SIGNAL(destroyed()), fng, SLOT(childChanged()));
  }
}

FootnoteItem::~FootnoteItem() {
}

bool FootnoteItem::setAutoContents() {
  return AutoNote::autoNote(data()->tag(), text(), style());
}

QGraphicsTextItem *FootnoteItem::tag() {
  return tag_;
}

void FootnoteItem::setTagText(QString t) {
  data()->setTag(t);
  updateTag();
}

QString FootnoteItem::tagText() const {
  return data()->tag();
}

void FootnoteItem::updateTag() {
  tag_->setPlainText(data()->tag() + ":");
  double textwidth = style().real("page-width")
    - style().real("margin-left")
    - style().real("margin-right");
  double tagwidth = tag_->boundingRect().width();
  text()->setPos(tagwidth, 0);
  text()->setTextWidth(textwidth - tagwidth);
  sizeToFit();
}

void FootnoteItem::abandon() {
  // Until we figure out how to unmark references, let's just not
  // delete abandoned notes.
}

