// TitleTextItem.C

#include "TitleTextItem.H"
#include "TitleData.H"

TitleTextItem::TitleTextItem(class TitleData *data, QGraphicsItem *parent):
  TextItem(data->versions().last(), parent),
  titleData_(data) {
  makeHardToWrite();
}

TitleTextItem::~TitleTextItem() {
}

bool TitleTextItem::makeWritable() {
  if (mayWrite && mayMark)
    return true;

  titleData_->revise();
  mayWrite = mayMark = true;
  return TextItem::makeWritable();
}

