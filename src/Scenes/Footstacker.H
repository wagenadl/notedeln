// Footstacker.H

#ifndef FOOTSTACKER_H

#define FOOTSTACKER_H

#include "BlockItem.H"
#include "FootnoteItem.H"

class Footstacker {
public:
  Footstacker(BlockItem *bi);
  int nBefore(double y);
public: // read only please
  QList<double> attach; // N entries, one per note
  QList<double> height; // N entries, one per note
  QList<FootnoteItem *> notes; // N entries, one per note
  QList<double> cumulHBefore; // N+1 entries, first is zero
private:
  BlockItem *bi;
  BlockData *bd;
  
};

#endif
