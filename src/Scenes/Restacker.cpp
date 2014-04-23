// Restacker.cpp

#include "Restacker.H"
#include "BlockItem.H"
#include "FocusProxyCache.H"
#include "Style.H"
#include "FootnoteItem.H"
#include "EntryScene.H"
#include "SheetScene.H"

Restacker::Restacker(QList<BlockItem *> const &blocks, int start):
  blocks(blocks), start(start) {
  end = start;

  if (blocks.isEmpty())
    return;
  
  y0 = blocks[0]->style().real("margin-top");
  y1 = blocks[0]->style().real("page-height")
    - blocks[0]->style().real("margin-bottom");

  yblock = y0; 
  yfn = y1; 
  isheet = 0; 

  if (start>0 && start<blocks.size()) {
    BlockData const *bd = blocks[start-1]->data();
    yblock = bd->y0() + bd->height();
    isheet = bd->sheet();
    foreach (FootnoteData const *fnd, bd->children<FootnoteData>()) 
      if (fnd->y0() < yfn)
	yfn = fnd->y0();
  }
}

void Restacker::restackData() {
  for (int i=start; i<blocks.size(); ++i) {
    bool chg = restackDatum(i);
    end = i+1;
    if (!chg
	&& end<blocks.size()
	&& blocks[end]->data()->y0()==yblock)
      break; // other stuff cannot be affected, right?
    // I think that footnotes may still be affected potentially
  }
}

bool Restacker::restackDatum(int i) {
  // This puts footnotedata on the right sheet, but not in the right order.
  // Create a sorting by yref?
  BlockItem *bi = blocks[i];
  BlockData *bd = bi->data();
  double blockh = bd->height();
  double fnh = 0;
  foreach (FootnoteData *fnd, bd->children<FootnoteData>()) 
    fnh += fnd->height();
  
  if (blockh+fnh > yfn-yblock) {
    // This block will not fit on the current sheet with its footnotes
    double ycut = bi->splittableY(yfn-yblock);
    if (ycut>0) {
      return restackDatumSplit(i, ycut);
    } else if (yblock>y0) {
      // Move to next sheet, except if we are the first block
      // on the current sheet.
      isheet++;
      yblock = y0;
      yfn = y1;
    }
  }

  // restack as one
  bool chg = false;
  if (bd->setSheetAndY0(isheet, yblock))
    chg = true;
  
  double yfn0 = yfn - fnh;
  foreach (FootnoteData *fnd, bd->children<FootnoteData>()) {
    if (fnd->setSheetAndY0(isheet, yfn0))
      chg = true;
    yfn0 += fnd->height();
  }
  yfn -= fnh;
  yblock += blockh;
  return chg;
}

bool Restacker::restackDatumSplit(int i, double ycut) {
  return false;
}
 
void Restacker::restackItems(EntryScene &es) {
  for (int i=start; i<end; i++) {
    restackItem(es, i);
  }
}

void Restacker::restackItem(EntryScene &es, int i) {
  BlockItem *bi = blocks[i];
  QGraphicsScene *s0 = bi->scene();
  QGraphicsScene *s1 = es.sheet(bi->data()->sheet(), true);
  if (s1!=s0) {
    FocusProxyCache fpc(bi);
    s1->addItem(bi);
    fpc.restore();
  }
  bi->resetPosition();
  
  foreach (FootnoteItem *fni, bi->footnotes()) {
    QGraphicsScene *s0 = fni->scene();
    QGraphicsScene *s1 = es.sheet(fni->data()->sheet(), true);
    if (s1!=s0) {
      FocusProxyCache fpc(fni);
      s1->addItem(fni);
      fpc.restore();
    }
    fni->resetPosition();
  }
}

