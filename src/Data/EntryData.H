// EntryData.H

#ifndef PAGEDATA_H

#define PAGEDATA_H

#include "Data.H"
#include <QList>

class EntryData: public Data {
  Q_OBJECT;
  Q_PROPERTY(int startPage READ startPage WRITE setStartPage)
public:
  // constructor and destructor  
  EntryData(Data *parent=0);
  virtual ~EntryData();
  // read properties
  int startPage() const;
  // write properties
  void setStartPage(int);
  // other
  class TitleData *title() const;
  QString titleText() const;
  QList<class BlockData *> blocks() const;
  void addBlock(BlockData *); // we become owner
  void insertBlockBefore(BlockData *newBlock, Data *ref); // we become owner
  bool deleteBlock(BlockData *);
  int sheetCount() const;
  void setBook(class Notebook *nb);      // } This is for use by items that 
  Notebook *book() const;                // } wish to use resources. We don't
  ResManager *resManager() const;          // } use it ourselves.
  EntryData const *page() const;
  EntryData *page();
  bool isEmpty() const; // true iff no blocks and title is default
signals:
  void titleMod();
  void sheetCountMod();
protected:
  virtual void loadMore(QVariantMap const &src);
private slots:
  void newSheet();
protected:
  int startPage_;
  TitleData *title_;
  int maxSheet;
  Notebook *nb;
};

#endif
