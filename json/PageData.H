// PageData.H

#ifndef PAGEDATA_H

#define PAGEDATA_H

#include "Data.H"
#include <QList>

class PageData: public Data {
  Q_OBJECT;
  Q_PROPERTY(int startPage READ startPage WRITE setStartPage)
  Q_PROPERTY(QString title READ title WRITE setTitle)
public:
  // constructor and destructor  
  PageData(Data *parent=0);
  virtual ~PageData();
  // read properties
  int startPage() const;
  QString title() const;
  // write properties
  void setStartPage(int);
  void setTitle(QString);
  // other
  QList<class BlockData *> const &blocks() const;
  void addBlock(BlockData *); // we become owner
  bool deleteBlock(BlockData *);
protected:
  virtual void loadMore(QVariantMap const &src);
  virtual void saveMore(QVariantMap &dst) const;
protected:
  int startPage_;
  QString title_;
  QList<class BlockData *> blocks_;
};

#endif
