// PageData.H

#ifndef PAGEDATA_H

#define PAGEDATA_H

#include "Data.H"
#include <QList>

class PageData: public Data {
  Q_OBJECT;
public:
  // constructor and destructor  
  PageData(Data *parent=0);
  virtual ~PageData();
  // read properties
  QList<class BlockData *> const &blocks() const;
  // write properties - not for casual use
  void setBlocks(QList<class BlockData *> const &);
  // other
  void addBlock(BlockData *);
protected:
  virtual void loadMore(QVariantMap const &src);
  virtual void saveMore(QVariantMap &dst) const;
protected:
  QList<class BlockData *> blocks_;
};

#endif
