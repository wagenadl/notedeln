// TitleItem.C

#include "TitleItem.H"
#include "Style.H"
#include <QDebug>
#include <QTextDocument>

TitleItem::TitleItem(TitleData *data, Item *parent):
  TextItem(data->current(), parent), data_(data) {
  qDebug() << "TitleItem!";

  setFont(QFont(style("title-font-family").toString(),
		style("title-font-size").toDouble()));
  setDefaultTextColor(QColor(style("title-color").toString()));
  setAllowParagraphs(false);
}


TitleItem::~TitleItem() {
}

TitleData *TitleItem::data() const {
  return data_;
}

void TitleItem::docChange() {
  QString plainText = toPlainText();
  if (data_->current()->text() != plainText)
    data_->revise();

  TextItem::docChange();
}
 
void TitleItem::focusInEvent(QFocusEvent *e) {
  TextItem::focusInEvent(e);
  if (toPlainText()==TitleData::defaultTitle()) {
    QTextCursor c = textCursor(); //(document());
    c.setPosition(0);
    c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    setTextCursor(c);
  }
}
