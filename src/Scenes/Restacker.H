// Restacker.H

#ifndef RESTACKER_H

#define RESTACKER_H

#include <QList>

class Restacker {
public:
  Restacker(QList<class BlockItem *> const &blocks, int start);
  void restackData();
  void restackItems(class EntryScene &es);
private:
  bool restackDatum(int i);
  bool restackDatumSplit(int i, double ysplit);
  void restackItem(EntryScene &es, int i);
public:
  QList<BlockItem *> const &blocks;
  int start;
  int end;
  double y0;
  double y1;
  double yblock; // top of next block
  double yfn; // bottom of next footnote
  int isheet; // sheet for next block & footnote
};

#endif
