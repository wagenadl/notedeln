// TextBlockItem.C

#include "TextBlockItem.H"
#include "TextBlockTextItem.H"
#include "TextBlockData.H"
#include "Style.H"
#include <QCursor>

TextBlockItem::TextBlockItem(TextBlockData *data, PageScene *parent):
  BlockItem(data, parent),
  data_(data) {
  setPos(Style::defaultStyle()["margin-left"].toDouble(), 0);
  item_ = new TextBlockTextItem(data_, this);
  setMainChild(item_);
  connect(item_, SIGNAL(textChanged()),
	  this, SLOT(checkVbox()));
}

TextBlockItem::~TextBlockItem() {
  // I assume the item_ is deleted by Qt?
}

TextBlockData *TextBlockItem::data() {
  return data_;
}

void TextBlockItem::setFocus() {
  item_->setFocus();
  item_->setCursor(QCursor(Qt::IBeamCursor));
}

//void TextBlockItem::dropFocus() {
  //  item_->unsetCursor();
//}
