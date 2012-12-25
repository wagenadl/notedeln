// TitleItem.C

#include "TitleItem.H"
#include "Style.H"
#include <QDebug>
#include <QTextDocument>

TitleItem::TitleItem(TitleData *data, Item *parent):
  TextItem(data->current(), parent), data_(data) {

  setFont(style().font("title-font"));
  setDefaultTextColor(style().color("title-color"));
  setAllowParagraphs(false);
 setZValue(10); // stack before other stuff so that late notes go in front
}


TitleItem::~TitleItem() {
}

TitleData *TitleItem::data() const {
  return data_;
}

void TitleItem::docChange() {
  QString plainText = text->toPlainText();
  if (data_->current()->text() != plainText)
    data_->revise();

  TextItem::docChange();
}
 
bool TitleItem::focusIn(QFocusEvent *) {
  if (text->toPlainText()==TitleData::defaultTitle()) {
    QTextCursor c = textCursor(); //(document());
    c.setPosition(0);
    c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    setTextCursor(c);
  }
  return false;
}
