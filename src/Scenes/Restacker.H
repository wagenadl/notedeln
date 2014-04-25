// Restacker.H

#ifndef RESTACKER_H

#define RESTACKER_H

#include <QList>
#include <QMap>
#include <QMultiMap>
#include <QSet>

class Restacker {
public:
  Restacker(QList<class BlockItem *> const &blocks, int start);
  void restackData();
  void restackItems(class EntryScene &es);
private:
  void restackBlocks();
  void restackFootnotesOnSheet();
  void restackBlock(int i);
  void restackBlockSplit(int i, double ysplit);
  void restackBlockOne(int i);
  void restackItem(EntryScene &es, int i);
private:
  QList<BlockItem *> const &blocks;
  int start;
  int end;
  double y0;
  double y1;
  double yblock; // top of next block
  double yfn; // bottom of next footnote
  int isheet; // sheet for next block & footnote
  QMap<int, QMultiMap<double, class FootnoteItem *> > footplace;
  // Maps sheet numbers to a map of reference positions to footnotes.
  // A reference position is the vertical position of the referring text
  // plus 0.001 * the horizontal position to break ties b/w multiple
  // references on the same line.
  QSet<int> changedSheets;
};

#endif
