// Restacker.cpp

#include "Restacker.H"
#include "BlockItem.H"
#include "FocusProxyCache.H"
#include "Style.H"
#include "FootnoteItem.H"
#include "EntryScene.H"
#include "SheetScene.H"
#include <QDebug>

Restacker::Restacker(QList<BlockItem *> const &blocks, int s):
  blocks(blocks), start(s) {
  qDebug() << "Restacker: start requested is " << start;
  ASSERT(start>=0);
  end = start;
  if (start>=blocks.size())
    return;
  
  y0 = blocks[0]->style().real("margin-top");
  y1 = blocks[0]->style().real("page-height")
    - blocks[0]->style().real("margin-bottom");

  /* We need to place our block below the previous block, but we need
     to restack all the notes on this sheet. If our sheet number differs
     from the previous block's, we need to consider whether we can move
     back. That involves looking at the footnotes on the previous page,
     so that implies restacking the entire previous page.
  */
  
  yblock = y0; 
  yfn = y1;

  if (start>0)
    --start;
  // Now "start" points to a block that certainly will not move, even though
  // its footnotes may move.
  isheet = blocks[start]->data()->sheet();
  if (isheet<0) {
    isheet = 0;
    start = 0;
  }

  // back to first block on sheet
  while (start>0) 
    if (blocks[start-1]->data()->sheet()!=isheet)
      break;
    else
      --start;
  qDebug() << "Start using is " << start;
}

void Restacker::restackData() {
  restackBlocks();
}

void Restacker::restackBlocks() {
  for (int i=start; i<blocks.size(); ++i) {
    int sh0 = isheet;
    restackBlock(i);
    if (isheet==sh0 || changedSheets.contains(isheet)
	|| (i+1<blocks.size() && (blocks[i+1]->data()->sheet()==isheet
				  || blocks[i+1]->data()->sheet()<0)))
      end = i+1;
    else
      break; // other stuff cannot be affected
  }
  restackFootnotesOnSheet();
}

void Restacker::restackBlock(int i) {
  // This puts footnotedata on the right sheet, but not in the right order.
  // Create a sorting by yref?
  BlockItem *bi = blocks[i];
  BlockData *bd = bi->data();
  double blockh = bd->height();
  double fnh = 0;
  foreach (FootnoteData *fnd, bd->children<FootnoteData>()) 
    fnh += fnd->height();

  qDebug() << "RestackBlock " << i << ": h=" << blockh << "+" << fnh;
  qDebug() << "  y=" << yblock << " ... " << yfn;
  
  if (blockh+fnh > yfn-yblock) {
    qDebug() << "  Will not fit";
    // This block will not fit on the current sheet with its footnotes
    double ycut = bi->splittableY(yfn-yblock);
    if (ycut>0) {
      restackBlockSplit(i, ycut);
      return;
    } else if (yblock>y0) {
      // Move to next sheet, except if we are the first block
      // on the current sheet.
      restackFootnotesOnSheet();
      isheet++;
      yblock = y0;
      yfn = y1;
    }
  }
  restackBlockOne(i);
}

void Restacker::restackBlockOne(int i) {
  BlockItem *bi = blocks[i];
  BlockData *bd = bi->data();
  if (bd->setSheetAndY0(isheet, yblock))
    changedSheets.insert(isheet);
  qDebug() << "RestackBlockOne " << i << ": " << isheet << ":"<<yblock
	   << " " << (changedSheets.contains(isheet) ? "*" : "");

  foreach (FootnoteItem *fni, bi->footnotes()) {
    QPointF p = bi->findRefText(fni->data()->tag());
    double rp = yblock + p.y() + 0.001*p.x();
    footplace[isheet].insert(rp, fni);
    yfn -= fni->data()->height();
  }
  yblock += bd->height();
}

void Restacker::restackBlockSplit(int /*i*/, double /*ycut*/) {
  //
}

void Restacker::restackFootnotesOnSheet() {
  double y1 = blocks[0]->style().real("page-height")
    - blocks[0]->style().real("margin-bottom");
  foreach (FootnoteItem *fni, footplace[isheet]) 
    y1 -= fni->data()->height();
  foreach (FootnoteItem *fni, footplace[isheet]) {
    fni->data()->setSheetAndY0(isheet, y1);
    y1 += fni->data()->height();
  }
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

