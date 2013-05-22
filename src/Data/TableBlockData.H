// TableBlockData.H

#ifndef TABLEBLOCKDATA_H

#define TABLEBLOCKDATA_H

#include "TextBlockData.H"

class TableBlockData: public TextBlockData {
  Q_OBJECT;
public:
  TableBlockData(Data *parent=0);
  virtual ~TableBlockData();
  class TableData const *table() const;
  virtual bool isEmpty() const;
  class TableData *table();
protected:
  void loadMore(QVariantMap const &src);
private:
  class TableData *table_;
};

#endif
