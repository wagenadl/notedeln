// TextBlockData.H

#ifndef TEXTBLOCKDATA_H

#define TEXTBLOCKDATA_H

#include "BlockData.H"
#include "TextData.H"

class TextBlockData: public BlockData {
  Q_OBJECT;
public:
  TextBlockData(class PageData *parent=0);
  virtual ~TextBlockData();
  TextData const *text() const;
  TextData *text();
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
protected:
  TextData *text_;
};

#endif
