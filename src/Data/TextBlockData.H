// TextBlockData.H

#ifndef TEXTBLOCKDATA_H

#define TEXTBLOCKDATA_H

#include "BlockData.H"
#include "TextData.H"

class TextBlockData: public BlockData {
  Q_OBJECT;
public:
  TextBlockData(Data *parent=0);
  virtual ~TextBlockData();
  TextData const *text() const;
  TextData *text();
  virtual bool isEmpty() const;
protected:
  void loadMore(QVariantMap const &src);
private:
  TextData *text_;
};

#endif
