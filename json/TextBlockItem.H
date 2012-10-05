// TextBlockItem.H

#ifndef TEXTBLOCKITEM_H

#define TEXTBLOCKITEM_H

#include "BlockItem.H"

class TextBlockItem: public BlockItem {
public:
  TextBlockItem(class TextBlockData *data, class PageScene *parent);
  ~TextBlockItem();
  TextBlockData *data(); // benevolently hides inherited version
  void setFocus();
private:
  TextBlockData *data_; // we don't own
  class TextBlockTextItem *item_; // we do own
};

#endif
