// PageData.H

#ifndef PAGEDATA_H

#define PAGEDATA_H

#include "Data.H"
#include <QList>

class PageData: public Data {
  Q_OBJECT;
  Q_PROPERTY(int startPage READ startPage WRITE setStartPage)
public:
  // constructor and destructor  
  PageData(Data *parent=0);
  virtual ~PageData();
  // read properties
  int startPage() const;
  // write properties
  void setStartPage(int);
  // other
  class TitleData *title() const;
  QList<class BlockData *> const &blocks() const;
  void addBlock(BlockData *); // we become owner
  bool deleteBlock(BlockData *);
  int sheetCount() const;
signals:
  void titleMod();
  void sheetCountMod();
protected:
  virtual void loadMore(QVariantMap const &src);
  virtual void saveMore(QVariantMap &dst) const;
private slots:
  void newSheet();
protected:
  int startPage_;
  TitleData *title_;
  QList<class BlockData *> blocks_;
  int maxSheet;
};

#endif
