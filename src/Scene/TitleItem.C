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
 
