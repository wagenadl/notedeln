// Restacker.cpp

#include "Restacker.H"
#include "BlockItem.H"
#include "FocusProxyCache.H"
#include "Style.H"
#include "FootnoteItem.H"
#include "EntryScene.H"
#include "SheetScene.H"
#include <QDebug>
#include "Footstacker.H"

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
  while (start>0) {
    if (blocks[start-1]->data()->sheet()!=isheet) {
      if (blocks[start-1]->data()->sheetSplits().isEmpty())
	break;
      isheet = blocks[start-1]->data()->sheet();
    }
    --start;
  }
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
				  || blocks[i+1]->data()->sheet()<0))
	|| true)
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
  /* We're about to restack a single block with its footnotes, already
     knowing that it will fit on the current sheet.
   */
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

  if (!bd->sheetSplits().isEmpty()) 
    bd->resetSheetSplits();
  bi->unsplit();

  yblock += bd->height();
}

void Restacker::restackBlockSplit(int i, double ycut) {
  /* We're about to restack a single block with its footnotes, but it will
     not fit on the current sheet. However, we do know that splitting is,
     at least in priciple, an option.
  */

  BlockItem *bi = blocks[i];
  BlockData *bd = bi->data();
  double hreal = bi->splittableY(bd->height()); // less than h b/c of b. marg.

  /* First, we must collect all the footnotes that belong to this block and
     sort them in order of y-reference. A line of the block may not be placed
     on the sheet if its footnotes do not also fit. (Except if this would cause
     an entire sheet to be empty, which is a special case.
  */

  QList<double> cuts;
  Footstacker fs(bi);
  double lastycut = 0;
  int lastn = 0;
  while (lastycut<hreal) {
    int nextn;
    double spaceav;
    double blockneed;
    double noteneed;
    double over;
    while (true) {
      nextn = fs.nBefore(ycut);
      spaceav = yfn-yblock;
      blockneed = ycut-lastycut;
      noteneed = fs.cumulHBefore[nextn]-fs.cumulHBefore[lastn];
      over = blockneed + noteneed - spaceav;
      if (over<=0)
	break; // we fit

      /* Unfortunately, our selected text plus associated notes will not fit.
	 We'll have to drop some text.
	 We could try to be smart and see if the last footnote simply must be
	 dropped, but for now, let's just peel off one line at a time.
      */
      if (nextn>lastn) {
	double ynote = fs.attach[nextn-1];
	if (over > ycut - ynote) {
	  // have to lose the note
	  ycut = bi->splittableY(ynote-5.0);
	  continue;
	}
      }
      ycut = bi->splittableY(ycut - over);
    }
    if (ycut<=lastycut) {
      // nothing fits at all
      if (yblock>y0) {
	// this sheet is not empty, so we'll move on
	restackFootnotesOnSheet();
	isheet++;
	yblock = y0;
	yfn = y1;
	ycut = bi->splittableY(lastycut + yfn-yblock);
	continue; // try again on next sheet
      } else {
	// we'll just have to make it work
	if (lastn < fs.notes.size()) {
	  // we have a footnote
	  nextn = lastn + 1;
	  ycut = bi->splittableY(fs.attach[lastn]+20); // well... what can I say
	} else {
	  qDebug() << "Restacker: I didn't think this could happen";
	  ycut = y1-y0;
	  nextn = lastn;
	}
      }
    }
    // we can fit at least a little. ycut and nextn are well defined
    if (lastycut==0) {
      // we're doing the first bit
      if (bd->setSheetAndY0(isheet, yblock))
	changedSheets.insert(isheet);
    }
    yblock += ycut-lastycut;
    cuts << ycut;
    for (int n=lastn; n<nextn; n++) {
      footplace[isheet].insert(fs.attach[n], fs.notes[n]);
      yfn -= fs.height[n];
    }
    lastycut = ycut;
    lastn = nextn;
    ycut = bd->height(); // let's see what we can do next
  }

  if (lastn<fs.notes.size()) {
    qDebug() << "Some notes not yet handled!";
    ASSERT(0);
  }

  ASSERT(!cuts.isEmpty());
  cuts.pop_back(); // the last is the height, by def.
  if (cuts!=bd->sheetSplits()) {
    bd->setSheetSplits(cuts);
    for (int k=0; k<cuts.size(); k++)
      changedSheets.insert(isheet-cuts.size()+k);
  }
  bi->split(cuts);
  yblock += bd->height() - lastycut; // reapply bottom margin
}

void Restacker::restackFootnotesOnSheet() {
  double y = blocks[0]->style().real("page-height")
    - blocks[0]->style().real("margin-bottom");
  foreach (FootnoteItem *fni, footplace[isheet]) 
    y -= fni->data()->height();
  foreach (FootnoteItem *fni, footplace[isheet]) {
    if (fni->data()->setSheetAndY0(isheet, y))
      changedSheets.insert(isheet);
    y += fni->data()->height();
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

  QList<double> cuts = bi->data()->sheetSplits();
  int nfrag = cuts.size() + 1;
  for (int k=1; k<nfrag; k++) {
    Item *ti = bi->fragment(k);
    ti->setPos(ti->style().real("margin-left"),
	       y0 - cuts[k-1]); // is that right?
    QGraphicsScene *s0 = ti->scene();
    QGraphicsScene *s1 = es.sheet(bi->data()->sheet()+k, true);
    if (s1!=s0) {
      FocusProxyCache fpc(ti);
      s1->addItem(ti);
      fpc.restore();
    }
  }
  
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

