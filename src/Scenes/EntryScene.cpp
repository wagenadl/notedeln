// Scenes/EntryScene.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// EntryScene.C

#include "EntryScene.h"

#include "EventView.h"
#include "DragLine.h"
#include "SheetScene.h"
#include "Style.h"
#include "BlockData.h"
#include "BlockItem.h"
#include "TitleItem.h"
#include "TextBlockItem.h"
#include "TableBlockItem.h"
#include "TextBlockData.h"
#include "TableBlockData.h"
#include "EntryData.h"
#include "GfxBlockItem.h"
#include "GfxBlockData.h"
#include "ResManager.h"
#include "Mode.h"
#include "FootnoteData.h"
#include "FootnoteItem.h"
#include "Assert.h"
#include "Notebook.h"
#include "LateNoteItem.h"
#include "GfxNoteItem.h"
#include "GfxNoteData.h"
#include "TableItem.h"
#include "LateNoteManager.h"

#include "SvgFile.h"
#include "Restacker.h"
#include "Cursors.h"

#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QDateTime>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextLine>
#include <QTextLayout>
#include <QTextBlock>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QSignalMapper>
#include <QCursor>
#include <QUrl>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>

EntryScene::EntryScene(CachedEntry data, QObject *parent):
  BaseScene(data, parent),
  data_(data) {
  writable = false;
  firstDisallowedPgNo = 0;
  
  unlockedItem = 0;
  lateNoteParent = 0;

  vChangeMapper = new QSignalMapper(this);
  futileMovementMapper = new QSignalMapper(this);
  connect(vChangeMapper, SIGNAL(mapped(int)), SLOT(vChanged(int)));
  connect(futileMovementMapper, SIGNAL(mapped(int)), SLOT(futileMovement(int)));
}

void EntryScene::populate() {
  BaseScene::populate();
  makeBlockItems();
  positionBlocks();
  loadLateNotes();

  if (data()->isUnlocked())
    addUnlockedWarning();
}

QDate EntryScene::date() const {
  return data_->created().date();
}

void EntryScene::loadLateNotes() {
  for (LateNoteData *lnd: data_.lateNoteManager()->notes()) {
    if (lnd->text()->text().isEmpty()) {
      data_.lateNoteManager()->takeChild(lnd);
      qDebug() << "Dropping empty late note";
    } else {
      LateNoteItem *lni = new LateNoteItem(lnd);
      sheets[lnd->sheet()]->addItem(lni);
      qDebug() << "Created LNI";
      qDebug() << lni << lni->data() << lni->data()->parent() << lni->parent();
    }
  }
}

void EntryScene::makeBlockItems() {
  foreach (BlockData *bd, data_->blocks()) {
    BlockItem *bi = tryMakeTableBlock(bd);
    if (!bi)
      bi = tryMakeTextBlock(bd);
    if (!bi)
      bi = tryMakeGfxBlock(bd);
    ASSERT(bi);
    connect(bi, SIGNAL(heightChanged()), vChangeMapper, SLOT(map()));
    blockItems.append(bi);
  }
  redateBlocks();
  remap();
}

BlockItem *EntryScene::tryMakeGfxBlock(BlockData *bd) {
  GfxBlockData *gbd = dynamic_cast<GfxBlockData*>(bd);
  if (!gbd)
    return 0;
  GfxBlockItem *gbi = new GfxBlockItem(gbd);
  gbi->setBaseScene(this);
  if (gbd->height()==0)
    gbi->sizeToFit();
  return gbi;
}

BlockItem *EntryScene::tryMakeTableBlock(BlockData *bd) {
  TableBlockData *tbd = dynamic_cast<TableBlockData*>(bd);
  if (!tbd)
    return 0;
  TableBlockItem *tbi = new TableBlockItem(tbd);
  tbi->setBaseScene(this);
  if (tbd->height()==0)
    tbi->sizeToFit();
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  connect(tbi, SIGNAL(sheetRequest(int)), this, SIGNAL(sheetRequest(int)));
  connect(tbi, SIGNAL(unicellular(class TableData *)),
	  SLOT(makeUnicellular(class TableData *)),
	  Qt::QueuedConnection);
  return tbi;
}

BlockItem *EntryScene::tryMakeTextBlock(BlockData *bd) {
  TextBlockData *tbd = dynamic_cast<TextBlockData*>(bd);
  if (!tbd)
    return 0;
  TextBlockItem *tbi = new TextBlockItem(tbd);
  tbi->setBaseScene(this);
  if (tbd->height()==0)
    tbi->sizeToFit();
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  connect(tbi, SIGNAL(sheetRequest(int)), this, SIGNAL(sheetRequest(int)));
  connect(tbi, SIGNAL(multicellular(int, class TextData *)),
	  SLOT(makeMulticellular(int, class TextData *)),
	  Qt::QueuedConnection);
  connect(tbi, SIGNAL(multicellularpaste(class TextData *, QString)),
	  SLOT(makeMulticellularAndPaste(class TextData *, QString)),
	  Qt::QueuedConnection);
    
  return tbi;
}
  
EntryScene::~EntryScene() {
}

void EntryScene::titleEdited() {
  foreach (SheetScene *s, sheets)
    s->repositionTitle();
//  TOCEntry *te = data()->book()->toc()->entry(data()->startPage());
//  ASSERT(te);
//  te->setTitle(title());
}

TitleData *EntryScene::fancyTitle() const {
  return data()->title();
}

void EntryScene::positionBlocks() {
  int isheet = 0;
  QSet<int> refootsheets;
  foreach (BlockItem *bi, blockItems) {
    int ish = bi->data()->sheet();
    if (ish>=0)
      isheet = ish;
    else
      qDebug() << "Loaded block with -ve sheet no";
    sheet(isheet, true)->addItem(bi);
    bi->resetPosition();
    
    QList<double> ycut = bi->data()->sheetSplits();
    for (int i=0; i<ycut.size(); i++) {
      bi->fragment(i+1)->setPos(style().real("margin-left"),
			      style().real("margin-top") - ycut[i]);
      sheet(isheet+i+1, true)->addItem(bi->fragment(i+1));
    }

    foreach (FootnoteItem *fni, bi->footnotes()) {
      int jsheet = fni->data()->sheet();
      if (jsheet<0) {
	jsheet = isheet;
	refootsheets.insert(isheet);
      } 
      sheet(jsheet, true)->addItem(fni);
      fni->resetPosition();
    }
  }
  foreach (int jsheet, refootsheets)
    Restacker::sneakilyRepositionNotes(blockItems, jsheet);    
  
  resetSheetCount();
}

void EntryScene::restackBlocks(int start) {
  if (!writable)
    return;

  Restacker restacker(blockItems, start);
  restacker.restackData();
  restacker.restackItems(*this);

  redateBlocks();
  resetSheetCount();
  emit restacked();
}

void EntryScene::resetSheetCount() {
  int maxsheet = 0;
  foreach (BlockData *bd, data()->children<BlockData>()) {
    int s = bd->sheet() + bd->sheetSplits().size();
    if (s>maxsheet)
      maxsheet = s;
    foreach (FootnoteData *fnd, bd->children<FootnoteData>()) 
      if (fnd->sheet()>maxsheet)
	maxsheet = fnd->sheet();
  }
  setSheetCount(maxsheet + 1);
}

void EntryScene::setSheetCount(int n) {
  BaseScene::setSheetCount(n);
//  TOCEntry *te = data()->book()->toc()->entry(data()->startPage());
//  ASSERT(te);
//  te->setSheetCount(n);
}

void EntryScene::redateBlocks() {
  QDateTime cre = data()->created();
  QDateTime mod = cre;
  QMap<BlockItem *, QString> txt;
  bool first = true;
  int st = data()->stampTime();
  double tmin = style().real("auto-timestamp-min-dt"); 
  for (int i=0; i<blockItems.size(); i++) {
    if (blockItems[i]->data()->isEmpty() && i<blockItems.size()-1)
      continue;
    QDateTime cre1 = blockItems[i]->data()->created();
    QDateTime mod1 = blockItems[i]->data()->modified();
    if (cre1.date()==cre.date() && mod1.date()==mod.date()) {
      // at most a time difference
      if (st>0 || (st==0 && (tmin>0
			     && (cre.secsTo(cre1) >= tmin
				 || cre.secsTo(cre1) < 0 || first)))) {
	QString txt1 = cre1.toString("h:mm ap");
	if (i>0 && (cre.time().hour()>=12 || cre1.time().hour()<12)) {
	  // no need for am/pm
	  //	  txt1 = txt1.left(txt1.length()-3);
	}
	if (cre.secsTo(cre1)<0)
	  txt1 += " (!)";
	txt[blockItems[i]] = txt1;
      }
    } else {
      QString txt1;
      if (cre1.date().year()==cre.date().year())
        txt1 = cre1.toString(style().string("date-format-yearless"));
      else
        txt1 = cre1.toString(style().string("date-format"));
      if (mod1.date()!=cre1.date()) {
        // date range
        txt1 += QString::fromUtf8("‒"); // figure dash
        if (mod1.date().year()==cre1.date().year())
	  txt1 += mod1.toString(style().string("date-format-yearless"));
        else
	  txt1 += mod1.toString(style().string("date-format"));
      }
      txt[blockItems[i]] = txt1;
    }
    cre = cre1;
    mod = mod1;
    first = false;
  }

  foreach (BlockItem *i, blockDateItems.keys()) {
    if (!txt.contains(i)) {
      delete blockDateItems[i];
      blockDateItems.remove(i);
    }
  }
  foreach (BlockItem *i, txt.keys()) {
    QGraphicsTextItem *dateItem = blockDateItems[i];
    if (!dateItem) {
      dateItem = blockDateItems[i] = new QGraphicsTextItem(i);
      dateItem->setFont(style().font("timestamp-font"));
      dateItem->setDefaultTextColor(style().color("timestamp-color"));
    }
    dateItem->setPlainText(txt[i]);
    QRectF br = i->sceneBoundingRect();
    QPointF bs0 = i->scenePos();
    QRectF dr = dateItem->sceneBoundingRect();
    QPointF ds0 = dateItem->scenePos();
    double ml = style().real("margin-left");
    dateItem->setPos(QPointF(ml - dr.width() - 2 - bs0.x(),
			     (br.top() - bs0.y()) - (dr.top() - ds0.y())
			     + style().real("text-block-above") - 4));
  }    
}

void EntryScene::notifyChildless(BlockItem *gbi) {
  if (!dynamic_cast<GfxBlockItem*>(gbi))
    return;
  if (!writable)
    return;
  int iblock = -1;
  for (int i=0; i<blockItems.size(); ++i) {
    if (blockItems[i] == gbi) {
      deleteBlock(i);
      iblock = i;
      break;
    }
  }

  if (iblock>=0)
    restackBlocks(iblock);
}
  
void EntryScene::deleteBlock(int blocki) {
  if (blocki>=blockItems.size()) {
    qDebug() << "EntryScene: deleting nonexisting block " << blocki;
    return;
  }
  BlockItem *bi = blockItems[blocki];
  BlockData *bd = bi->data();

  blockItems.removeAt(blocki);
  remap();

  foreach (FootnoteItem *fni, bi->footnotes()) {
    QGraphicsScene *s = fni->scene();
    if (s)
      s->removeItem(fni);
    fni->deleteLater();
  }

  QGraphicsScene *s = bi->scene();
  if (s)
    s->removeItem(bi);
  bi->unsplit();
  bi->deleteLater();

  data_->deleteBlock(bd);
}

GfxBlockItem *EntryScene::newGfxBlock(int iAbove) {
  int iNew = (iAbove>=0)
    ? iAbove + 1
    : 0; // blockItems.size();

  // if (iAbove>=0) {
  //   // perhaps not create a new one after all
  //   GfxBlockItem *tbi = dynamic_cast<GfxBlockItem *>(blockItems[iAbove]);
  //   if (tbi && tbi->isWritable()) {
  //     // Previous block is writable, use it instead
  //     return tbi;
  //   }
  // }

  GfxBlockData *gbd = new GfxBlockData();
  QList<BlockData *> existingBlocks = data_->blocks();
  if (iNew<existingBlocks.size())
    data_->insertBlockBefore(gbd, existingBlocks[iNew]);
  else
    data_->addBlock(gbd);
  GfxBlockItem *gbi = new GfxBlockItem(gbd);
  gbi->setBaseScene(this);
  gbi->sizeToFit();
  gbi->makeWritable();
  
  blockItems.insert(iNew, gbi);

  connect(gbi, SIGNAL(heightChanged()), vChangeMapper, SLOT(map()));
  remap();

  restackBlocks(iNew);
  gotoSheetOfBlock(iNew);
  return gbi;
}

void EntryScene::splitTextBlock(int iblock, int pos) {
  // block number iblock is going to be split
  TextBlockData *orig =
    dynamic_cast<TextBlockData*>(blockItems[iblock]->data());
  ASSERT(orig);
  TextBlockData *block1 = Data::deepCopy(orig);
  deleteBlock(iblock);
  TextBlockData *block2 = block1->split(pos);
  TextBlockItem *tbi_pre = injectTextBlock(block1, iblock);
  TextBlockItem *tbi_post = injectTextBlock(block2, iblock+1);
  tbi_pre->text()->document()->relayout();
  tbi_post->text()->document()->relayout();
  restackBlocks(iblock);
  gotoSheetOfBlock(iblock+1);
  tbi_post->setFocus();
}

void EntryScene::joinTextBlocks(int iblock_pre, int iblock_post,
				bool forward) {
  ASSERT(iblock_pre<iblock_post);
  ASSERT(iblock_pre>=0);
  ASSERT(iblock_post<blockItems.size());
  TextBlockItem *tbi_pre
    = dynamic_cast<TextBlockItem*>(blockItems[iblock_pre]);
  TextBlockItem *tbi_post
    = dynamic_cast<TextBlockItem*>(blockItems[iblock_post]);
  ASSERT(tbi_pre);
  ASSERT(tbi_post);
  if (dynamic_cast<TableBlockData*>(tbi_pre->data())
      || dynamic_cast<TableBlockData*>(tbi_post->data()))
    return; // don't try to merge tables
  TextBlockData *block1 = Data::deepCopy(tbi_pre->data());
  TextBlockData *block2 = Data::deepCopy(tbi_post->data());
  int pos = block1->text()->text().size();
  deleteBlock(iblock_post);
  deleteBlock(iblock_pre);
  block1->join(block2);
  block1->resetSheetSplits();
  TextBlockItem *tbi = injectTextBlock(block1,
				       forward ? iblock_post - 1 : iblock_pre);
  tbi->text()->document()->relayout();
  restackBlocks(iblock_pre);
  gotoSheetOfBlock(iblock_pre);
  tbi->setFocus();
  TextCursor c(tbi->document(), pos);
  tbi->setTextCursor(c);
}  

TableBlockItem *EntryScene::injectTableBlock(TableBlockData *tbd, int iblock) {
  // creates a new table block immediately before iblock (or at end if iblock
  // points past the last text block)
  BlockData *tbd_next =  iblock<blockItems.size()
    ? data_->blocks()[iblock]
    : 0;
  data_->insertBlockBefore(tbd, tbd_next);
  TableBlockItem *tbi = new TableBlockItem(tbd);
  tbi->setBaseScene(this);
  tbi->sizeToFit();
  tbi->makeWritable();

  blockItems.insert(iblock, tbi);
  connect(tbi, SIGNAL(heightChanged()), vChangeMapper, SLOT(map()));
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  connect(tbi, SIGNAL(sheetRequest(int)), this, SIGNAL(sheetRequest(int)));
  connect(tbi, SIGNAL(unicellular(class TableData *)),
	  SLOT(makeUnicellular(class TableData *)),
	  Qt::QueuedConnection);
  remap();
  return tbi;
}

TextBlockItem *EntryScene::injectTextBlock(TextBlockData *tbd, int iblock) {
  // creates a new text block immediately before iblock (or at end if iblock
  // points past the last text block)
  BlockData *tbd_next =  iblock<blockItems.size()
    ? data_->blocks()[iblock]
    : 0;
  data_->insertBlockBefore(tbd, tbd_next);
  TextBlockItem *tbi = new TextBlockItem(tbd);
  tbi->setBaseScene(this);
  tbi->sizeToFit();
  tbi->makeWritable();

  blockItems.insert(iblock, tbi);
  connect(tbi, SIGNAL(heightChanged()), vChangeMapper, SLOT(map()));
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  connect(tbi, SIGNAL(sheetRequest(int)), this, SIGNAL(sheetRequest(int)));
  connect(tbi, SIGNAL(multicellular(int, class TextData *)),
	  SLOT(makeMulticellular(int, class TextData *)),
	  Qt::QueuedConnection);
  connect(tbi, SIGNAL(multicellularpaste(class TextData *, QString)),
	  SLOT(makeMulticellularAndPaste(class TextData *, QString)),
	  Qt::QueuedConnection);
  remap();
  return tbi;
}

void EntryScene::remap() {
  for (int i=0; i<blockItems.size(); i++) {
    BlockItem *bi = blockItems[i];
    vChangeMapper->setMapping(bi, i);
    futileMovementMapper->setMapping(bi, i);
  }
}

TableBlockItem *EntryScene::newTableBlock(int iAbove) {
  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();

  TableBlockData *tbd = new TableBlockData();
  TableBlockItem *tbi = injectTableBlock(tbd, iNew);
  
  restackBlocks(iNew);
  gotoSheetOfBlock(iNew);
  tbi->setFocus();
  return tbi;
}

int EntryScene::lastBlockAbove(QPointF scenepos, int sheet) {
  int alt = blockItems.size() - 1;
  for (int i=0; i<blockItems.size(); i++) {
    if (blockItems[i]->data()->sheet() != sheet)
      continue;
    double y = blockItems[i]->sceneBoundingRect().bottom();
    if (y>scenepos.y())
      return i-1;
    else
      alt = i;
  }
  return alt;
}

TextBlockItem *EntryScene::newTextBlockAt(QPointF scenepos, int sheet,
                                          bool evenIfLastEmpty) {
  return newTextBlock(lastBlockAbove(scenepos, sheet), evenIfLastEmpty);
}

GfxBlockItem *EntryScene::newGfxBlockAt(QPointF scenepos, int sheet) {
  return newGfxBlock(lastBlockAbove(scenepos, sheet));
}

TableBlockItem *EntryScene::newTableBlockAt(QPointF scenepos, int sheet) {
  return newTableBlock(lastBlockAbove(scenepos, sheet));
}


TextBlockItem *EntryScene::newTextBlock(int iAbove, bool evenIfLastEmpty) {
  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();

  bool prevIsEmpty = false;
  if (iNew>0) {
    TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[iNew-1]);
    if (tbi && tbi->document()->isEmpty()) {
      prevIsEmpty = true;
      if (iAbove>=0 && tbi->isWritable() && !evenIfLastEmpty) {
        tbi->setFocus();
        return tbi;
      }
    }
  }

  TextBlockData *tbd = new TextBlockData(); // create w/o parent
  if (!style().flag("always-indent") && (iNew==0 || prevIsEmpty))
    tbd->setIndented(false);
  TextBlockItem *tbi = injectTextBlock(tbd, iNew); // this parents the block
  
  restackBlocks(iNew);
  gotoSheetOfBlock(iNew);
  tbi->setFocus();
  return tbi;
}

void EntryScene::futileMovement(int block) {
  //  qDebug() << "EntryScene::futileMovement" << block;
  ASSERT(block>=0 && block<blockItems.size());
  // futile movement in a text block
  TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[block]);
  if (!tbi) {
    qDebug() << "not a text block";
    return;
  }

  FutileMovementInfo fmi = tbi->lastFutileMovement();
  int tgtidx = -1;
  if (fmi.key()==Qt::Key_Enter || fmi.key()==Qt::Key_Return) {
    TextCursor c = tbi->textCursor();
    /* Ctrl-Enter makes next block with same indentation. Note that this
       currently cannot happen, because Ctrl-Enter is intercepted in TextItem
       and inserts a '\n' in the text. */
    if (c.atEnd() && !(fmi.modifiers() & Qt::ControlModifier)) 
      newTextBlock(block, true);
    else
      splitTextBlock(block, c.position());
    return;
  }
  
  if (fmi.key()==Qt::Key_Left || fmi.key()==Qt::Key_Up
      || fmi.key()==Qt::Key_Backspace) {
    // upward movement
    for (int b=block-1; b>=0; b--) {
      BlockItem *bi = blockItems[b];
      QString typ = bi->data()->type();
      if ((typ=="textblock" || typ=="tableblock") && bi->isWritable()) {
	tgtidx = b;
	break;
      }
    }
  } else if (fmi.key()==Qt::Key_Right || fmi.key()==Qt::Key_Down
	     || fmi.key()==Qt::Key_Delete) {
    // downward movement
    for (int b=block+1; b<blockItems.size(); b++) {
      BlockItem *bi = blockItems[b];
      QString typ = bi->data()->type();
      if ((typ=="textblock" || typ=="tableblock") && bi->isWritable()) {
	tgtidx = b;
	break;
      }
    }
  }

  if (tgtidx<0) {
    // no target, go to start/end of current
    TextCursor c = tbi->textCursor();
    if (fmi.key()==Qt::Key_Down) {
      c.movePosition(TextCursor::End,
		     fmi.modifiers() & Qt::ShiftModifier
		     ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
      tbi->setTextCursor(c);
    } else if (fmi.key()==Qt::Key_Up) {
      c.movePosition(TextCursor::Start,
		     fmi.modifiers() & Qt::ShiftModifier
		     ? TextCursor::KeepAnchor : TextCursor::MoveAnchor);
      tbi->setTextCursor(c);
    }
    return;
  }

  if (fmi.key()==Qt::Key_Delete) {
    if (tgtidx==block+1 // do not combine across (e.g.) gfxblocks
	|| blockItems[block]->data()->isEmpty()) // ... unless empty
      joinTextBlocks(block, tgtidx, true);
    return;
  } else if (fmi.key()==Qt::Key_Backspace) {
    if (tgtidx==block-1
	|| blockItems[block]->data()->isEmpty()) // ... unless empty
      joinTextBlocks(tgtidx, block);
    return;
  }
  
  TextBlockItem *tgt = dynamic_cast<TextBlockItem*>(blockItems[tgtidx]);
  ASSERT(tgt);

  /// Clear selection? Now handled by TextItemText!
  //foreach (TextItem *ti, tbi->fragments()) {
  //  QTextCursor c(ti->textCursor());
  //  if (c.hasSelection()) {
  //    c.clearSelection();
  //    ti->setTextCursor(c);
  //  }
  //  ti->clearFocus();
  //}
  
  TextItemDoc *doc = tgt->document();
  TextCursor c(doc);
  QPointF p = tgt->mapFromScene(fmi.scenePos());
  // Since we only use p.x, this calc is OK, even when fmi.scenePos was
  // in another fragment.
  switch (fmi.key()) {
  case Qt::Key_Left: 
    c.movePosition(TextCursor::End);
    break;
  case Qt::Key_Up: {
    int N = doc->lastPosition();
    QPointF endp = doc->locate(N);
    QPointF tgtp(p.x(), endp.y());
    int idx = doc->find(tgtp);
    c.setPosition(idx>=doc->firstPosition() ? idx : N);
    break; }
  case Qt::Key_Right:
    c.movePosition(TextCursor::Start);
    break;
  case Qt::Key_Down: {
    int n = doc->firstPosition();
    QPointF startp = doc->locate(n);
    QPointF tgtp(p.x(), startp.y());
    int idx = doc->find(tgtp);
    c.setPosition(idx>=n ? idx : n);
    break; }
  }
  tgt->setTextCursor(c);
}

void EntryScene::focusFirst(int isheet) {
  if (!writable)
    return;

  TextBlockItem *firstbi = 0;
  for (int i=0; i<blockItems.size(); ++i) {
    TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[i]);
    if (tbi && (isheet<0 || (isheet>=tbi->data()->sheet()
			    && isheet<=tbi->data()->sheet()
			    + tbi->data()->sheetSplits().size()))) {
      firstbi = tbi;
      break;
    }
  }
  if (firstbi) {
    firstbi->setFocus();
    TextCursor tc = firstbi->textCursor();
    tc.movePosition(TextCursor::Start);
    if (isheet>=0 && !firstbi->data()->sheetSplits().isEmpty()) {
      // make sure we pick the right fragment
      TextItem *i = firstbi->fragment(isheet-firstbi->data()->sheet());
      QRectF clip = i->clipRect();
      clip.setLeft(-10000);
      clip.setRight(10000); // I really don't care about x position
      while (!clip.contains(i->posToPoint(tc.position()))) {
	int p = tc.position();
	tc.movePosition(TextCursor::Down);
	tc.movePosition(TextCursor::StartOfLine);
	if (tc.position()==p)
	  break;
      }
    }      
    firstbi->text()->setTextCursor(tc);    
  } else {
    if (blockItems.isEmpty())
      newTextBlock(-1);
  }
}



void EntryScene::focusEnd(int isheet) {
  if (!writable)
    return;

  TextBlockItem *lastbi = 0;
  for (int i=0; i<blockItems.size(); ++i) {
    TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[i]);
    if (tbi && (isheet<0
		|| (isheet>=tbi->data()->sheet()
		    && isheet<=tbi->data()->sheet()
		    + tbi->data()->sheetSplits().size())))
      lastbi = tbi;
  }
  if (lastbi) {
    lastbi->setFocus();
    TextCursor tc = lastbi->textCursor();
    tc.movePosition(TextCursor::End);
    if (isheet>=0 && !lastbi->data()->sheetSplits().isEmpty()) {
      // make sure we pick the right fragment
      TextItem *i = lastbi->fragment(isheet-lastbi->data()->sheet());
      QRectF clip = i->clipRect();
      clip.setLeft(-10000);
      clip.setRight(10000); // I really don't care about x position
      while (!clip.contains(i->posToPoint(tc.position()))) {
	int p = tc.position();
	tc.movePosition(TextCursor::Up);
	tc.movePosition(TextCursor::EndOfLine);
	if (tc.position()==p)
	  break;
      }
    }      
    lastbi->setTextCursor(tc);    
  } else {
    if (isheet == nSheets-1)
      newTextBlock(blockItems.size()-1);
    // create new text block only on last sheet
  }
}


void EntryScene::vChanged(int block) {
  ASSERT(block>=0 && block<blockItems.size());
  TextBlockItem *tbi = dynamic_cast<TextBlockItem*>(blockItems[block]);
  if (tbi) {
    Item *f = 0;
    PageView *ev = EventView::eventView();
    if (ev)
      f = dynamic_cast<Item*>(ev->scene()->focusItem());
    // that's a really ugly way to find out who has focus
    TextCursor c = tbi->textCursor();
    restackBlocks(block);
    if (f && f->ancestralBlock()!=tbi)
      f->setFocus(); // restore focus to correct footnote. hope this works.
    /* It work more or less, but not in the context of cross-sheet notes. */
    else
      tbi->setTextCursor(c); // this ensures the correct fragment gets focus
  } else {
    restackBlocks(block);
    gotoSheetOfBlock(block);
  }
}

bool EntryScene::mousePressEvent(QGraphicsSceneMouseEvent *e, SheetScene *s) {
  QPointF sp = e->scenePos();
  int sh = findSheet(s);
  bool take = false;
  Item *it = 0;
  for (QGraphicsItem *gi = itemAt(sp, sh); gi; gi=gi->parentItem()) {
    it = dynamic_cast<Item*>(gi);
    if (it)
      break;
  }
  Mode *mo = sheets[sh]->mode();
  switch (mo->mode()) {
  case Mode::Mark: case Mode::Freehand:
    if (!it && isWritable() && !inMargin(sp)) {
      GfxBlockItem *blk = 0;
      int idx = findLastBlockOnSheet(sh);
      if (idx>=0)
        blk = dynamic_cast<GfxBlockItem *>(blockItems[idx]);
      if (!blk)
        blk = newGfxBlockAt(sp, sh);
      e->setPos(blk->mapFromScene(e->scenePos())); // brutal!
      blk->mousePressEvent(e);
      take = true;
    }
    break;
  case Mode::Type:
    if (!it && isWritable() && !inSideMargin(sp) && !inTopMargin(sp)) {
      newTextBlockAt(sp, sh);
      take = true;
    }
    break;
  case Mode::Annotate: {
    if (isWritable()) {
      if (!it || !it->makesOwnNotes())
	it = sheets[sh]->fancyTitleItem();
      GfxNoteItem *note = it->createGfxNote(it->mapFromScene(sp));
      note->data()->setSheet(sh);
    } else {
      createLateNote(sp, sh);
    }
    mo->setMode(Mode::Type);
    take = true;
  } break;
  default:
    break;
  }
  if (take)
    e->accept();
  return take;
}



bool EntryScene::keyPressEvent(QKeyEvent *e, SheetScene *s) {
  if (e->key()==Qt::Key_Tab || e->key()==Qt::Key_Backtab) {
    TextItem *focus = dynamic_cast<TextItem *>(s->focusItem());
    if (focus) {
      focus->keyPressEvent(e);
      e->accept();
      return true;
    }
  }
  if (e->modifiers() & Qt::ControlModifier) {
    bool steal = false;
    switch (e->key()) {
    case Qt::Key_V:
      if (writable)
	steal = tryToPaste(s);
      break;
    case Qt::Key_U:
      if (e->modifiers() & Qt::ShiftModifier) {
        unlock();
        steal = true;
      }
    }
    if (steal) {
      e->accept();
      return true;
    }
  }
  return false;
}

void EntryScene::unlock() {
  if (!writable) {
    data()->setUnlocked(true);
    makeWritable();
    addUnlockedWarning();
  }
}

void EntryScene::addUnlockedWarning() {
  if (unlockedItem)
    return;
  foreach (SheetScene *s, sheets) {
    unlockedItem = new QGraphicsTextItem();
    s->addItem(unlockedItem);
    unlockedItem->setPlainText(style().string("unlocked-text"));
    unlockedItem->setFont(style().font("unlocked-font"));
    unlockedItem->setDefaultTextColor(style().color("unlocked-color"));
    QRectF br = unlockedItem->sceneBoundingRect();
    unlockedItem->setPos(style().real("page-width") - br.width() - 4, 4);
  }
}

BlockItem const *EntryScene::findBlockByUUID(QString uuid) const {
  for (int i=0; i<blockItems.size(); i++) 
    if (blockItems[i]->data()->findDescendentByUUID(uuid))
      return blockItems[i];
  return 0;
}  

int EntryScene::findBlock(Data const *d0) const {
  BlockData const *bd = 0;
  for (Data const *d=d0; d; d=d->parent()) {
    bd = dynamic_cast<BlockData const*>(d);
    if (bd)
      break;
  }
  if (!bd)
    return -1;
  for (int i=0; i<blockItems.size(); i++) 
    if (blockItems[i]->data() == bd)
      return i;
  return -1;
}

int EntryScene::findBlock(Item const *i) const {
  BlockItem const *bi = i->ancestralBlock();
  for (int i=0; i<blockItems.size(); i++) 
    if (blockItems[i] == bi)
      return i;
  return -1;
}

int EntryScene::findBlock(QPointF scenepos, int sheet) const {
  for (int i=0; i<blockItems.size(); i++) {
    BlockData const *bd = blockItems[i]->data();
    if (bd->sheet() != sheet)
      continue;
    double y0 = bd->y0();
    if (scenepos.y()>=y0 && scenepos.y()<=y0+bd->height())
      return i;
  }
  return -1;
}

bool EntryScene::dropEvent(QGraphicsSceneDragDropEvent *e, SheetScene *s) {
  QMimeData const *md = e->mimeData();
  QPointF scenePos = e->scenePos();
  int sheet = findSheet(s);
  if (md->hasImage())
    return importDroppedImage(scenePos, sheet,
			    qvariant_cast<QImage>(md->imageData()),
			    QUrl());
  else if (md->hasUrls())
    return importDroppedUrls(scenePos, sheet, md->urls());
  else
    return false;
}


bool EntryScene::tryToPaste(SheetScene *s) {
  /* We get it first.
     If we don't send the event on to QGraphicsScene, textItems don't get it.
     What we do is dependent both on mimetype and on whether we have focus.
     If we have focus, text is not handled here, and images are placed in
     a new block below the focused block or in the focused block if it is a
     canvas.
     If we don't have focus, anything is placed in the focused block if it
     is a canvas, or in a new block if not.
  */
  qDebug() << "trytopaste";
  if (!isWritable())
    return false;
  
  QPointF scenePos;
  TextItem *fi = dynamic_cast<TextItem*>(s->focusItem());

  if (fi) {
    TextItemDoc *doc = fi->document();
    int pos = fi->textCursor().position();
    scenePos = fi->mapToScene(doc->locate(pos));
  } else {
    QList<QGraphicsView*> vv = s->views();
    if (vv.isEmpty()) {
      qDebug() << "EntryScene: cannot determine paste position: no view";
      return false;
    }
    if (vv.size()>1) {
      qDebug() << "EntryScene: multiple views: won't determine paste position";
      return false;
    }
    scenePos = vv[0]->mapToScene(vv[0]->mapFromGlobal(QCursor::pos()));
  }
  int sheet = findSheet(s);
  
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  qDebug() << "I/U/T" << md->hasImage() << md->hasUrls() << md->hasText();
  qDebug() << "fi" << fi;
  if (md->hasUrls())
    qDebug() << "u=" << md->urls();
  if (md->hasText())
    qDebug() << "t=" << md->text();

  /* The optimal behavior is actually quite tricky.
     In Linux, when I copy and paste:
     - an image file, I get a url and text
     - an image, I get an image
     - regular text, I get text
     - a piece of a spreadsheet, I get an image and text
   */

  if (md->hasUrls())
    return importDroppedUrls(scenePos, sheet, md->urls(), fi);
  else if (md->hasText())
    return importDroppedText(scenePos, sheet, md->text(), 0, 0, 0, fi);
  else if (md->hasImage())
    return importDroppedImage(scenePos, sheet,
			      qvariant_cast<QImage>(md->imageData()),
                              QUrl());
  else
    return false;
}

bool EntryScene::dropBelow(QPointF scenePos, int sheet, QMimeData const *md) {
  if (!isWritable())
    return false;
  if (md->hasImage())
    return importDroppedImage(scenePos, sheet,
			      qvariant_cast<QImage>(md->imageData()),
			      QUrl());
  else if (md->hasUrls())
    return importDroppedUrls(scenePos, sheet, md->urls());
  else if (md->hasText())
    return importDroppedText(scenePos, sheet, md->text());
  else
    return false;
}

bool EntryScene::importDroppedSvg(QPointF scenePos, int sheet,
				  QUrl const &source) {
  QImage img(SvgFile::downloadAsImage(source));
  if (img.isNull()) 
    return importDroppedText(scenePos, sheet, source.toString());
  else
    return importDroppedImage(scenePos, sheet, img, source);
}

bool EntryScene::importDroppedImage(QPointF scenePos, int sheet,
				    QImage const &img, QUrl const &source) {
  // Return true if we want it
  /* If dropped on an existing gfxblock, insert it there.
     If dropped on belowItem, insert after last block on page.
     If dropped on text block, insert after that text block.
     In the past, I did this:
     "Before creating a new graphics block, consider whether there is
     a graphics block right after it."
     But now I think that is not actually that useful. The issue was that
     adjacent GfxBlocks are hard to tell aprt, i.e., the boundary is nearly
     invisible. I now think that that visibility is the issue to be addressed.
   */
  QPointF pdest(0,0);

  int i = findBlock(scenePos, sheet);
  GfxBlockItem *gdst = (i>=0) ? dynamic_cast<GfxBlockItem*>(blockItems[i]) : 0;
  if (gdst) 
    pdest = gdst->mapFromScene(scenePos);
  // else if (i>=0 && i+1<blockItems.size())
  //   gdst = dynamic_cast<GfxBlockItem*>(blockItems[i+1]);
  if (!gdst)
    gdst = newGfxBlockAt(scenePos, sheet);

  gdst->newImage(img, source, pdest);
  gotoSheetOfBlock(findBlock(gdst));
  return true;
}

int EntryScene::indexOfBlock(BlockItem *bi) const {
  for (int i=0; i<blockItems.size(); ++i)
    if (blockItems[i]==bi)
      return i;
  return -1;
}

bool EntryScene::importDroppedUrls(QPointF scenePos, int sheet,
				   QList<QUrl> const &urls,
                                   TextItem *fi) {
  bool ok = false;
  foreach (QUrl const &u, urls)
    if (importDroppedUrl(scenePos, sheet, u, fi))
      ok = true;
  return ok;
}

bool EntryScene::importDroppedUrl(QPointF scenePos, int sheet,
				  QUrl const &url, TextItem *fi) {
  // QGraphicsItem *dst = itemAt(scenePos);
  /* A URL could be any of the following:
     (1) A local image file
     (2) A local file of non-image type
     (3) An internet-located image file
     (4) A web-page
     (5) Anything else
  */
  if (url.isLocalFile()) {
    QString path = url.toLocalFile();
    if (path.endsWith(".svg")) 
      return importDroppedSvg(scenePos, sheet, url);
    QImage image = QImage(path);
    if (!image.isNull())
      return importDroppedImage(scenePos, sheet, image, url);
    else 
      return importDroppedFile(scenePos, sheet, path);
  } else {
    // Right now, we import all network urls as text
    return importDroppedText(scenePos, sheet, url.toString(), 0, 0, 0, fi);
  }
  return false;
}

bool EntryScene::importDroppedText(QPointF scenePos, int sheet,
				   QString const &txt,
                                   TextItem **itemReturn,
                                   int *startReturn, int *endReturn,
                                   TextItem *fi) {
  if (fi)
    return false; // let item handle it instead
  TextItem *ti = 0;
  if (inMargin(scenePos)) {
    Item *fti = sheets[sheet]->fancyTitleItem();
    GfxNoteItem *note = fti->createGfxNote(fti->mapFromScene(scenePos));
    note->data()->setSheet(sheet);
    ti = note->textItem();
  } else {
    int blk = findBlock(scenePos, sheet);
    if (blk>=0) {
      if (blockItems[blk]->isWritable()) {
        GfxBlockItem *gbi = dynamic_cast<GfxBlockItem*>(blockItems[blk]);
        TextBlockItem *tbi = dynamic_cast<TextBlockItem*>(blockItems[blk]);
        if (tbi) {
          ti = tbi->text();
        } else if (gbi) {
          GfxNoteItem *note = gbi->createGfxNote(gbi->mapFromScene(scenePos));
          ti = note->textItem();
        }
      } else { // not writable block
	Item *fti = sheets[sheet]->fancyTitleItem();
	GfxNoteItem *note = fti->createGfxNote(fti->mapFromScene(scenePos));
	note->data()->setSheet(sheet);
        ti = note->textItem();
      }
    }
  }

  if (!ti)
    return false;

  if (itemReturn)
    *itemReturn = ti;
  
  TextCursor c = ti->textCursor();
  //  TextItemDoc *doc = ti->document();
  int pos = ti->pointToPos(ti->mapFromScene(scenePos));
  if (pos>=0)
    c.setPosition(pos);
  else
    c.clearSelection();
  if (startReturn)
    *startReturn = c.position();
  c.insertText(txt);
  if (endReturn)
    *endReturn = c.position();

  ti->setFocus();
  ti->setTextCursor(c);

  return true;
}

bool EntryScene::importDroppedFile(QPointF scenePos, int sheet,
				   QString const &fn) {
  if (!fn.startsWith("/"))
    return false;
  int start, end;
  TextItem *ti;
  if (!importDroppedText(scenePos, sheet, fn, &ti, &start, &end))
    return false;

  ti->addMarkup(MarkupData::Link, start, end);
  return true;
}

void EntryScene::resetCreation() {
  data()->resetCreation();
  redateBlocks();
  foreach (SheetScene *s, sheets)
    s->setDate(date());
}

void EntryScene::makeWritable() {
  if (data()->isEmpty())
    resetCreation();

  writable = true;
  //belowItem->setCursor(Qt::IBeamCursor);
  foreach (BlockItem *bi, blockItems)
    bi->makeWritable();
  foreach (SheetScene *s, sheets)
    s->fancyTitleItem()->makeWritable();
}

int EntryScene::startPage() const {
  return data_->startPage();
}

QString EntryScene::title() const {
  return data_->title()->text()->text();
}

bool EntryScene::isWritable() const {
  return writable;
}

bool EntryScene::focusFootnote(int block, QString tag) {
  ASSERT(block>=0 && block<blockItems.size());
  foreach (FootnoteItem *fni, blockItems[block]->footnotes()) {
    if (fni->data()->tag()==tag) {
      emit sheetRequest(fni->data()->sheet());
      fni->setFocus();
      return true;
    }
  }
  return false;
}

void EntryScene::newFootnote(int block, QString tag) {
  ASSERT(block>=0 && block<blockItems.size());
  if (focusFootnote(block, tag))
    return;
  FootnoteData *fnd = new FootnoteData(blockItems[block]->data());
  fnd->setTag(tag);
  FootnoteItem *fni = blockItems[block]->newFootnote(fnd);
  fni->makeWritable();
  fni->sizeToFit();
  restackBlocks(block);
  emit sheetRequest(fni->data()->sheet());
  fni->setAutoContents();
  fni->setFocus();
}

EntryData *EntryScene::data() const {
  return data_;
}

LateNoteItem *EntryScene::createLateNote(QPointF scenePos, int sheet) {
  QPointF sp1 = DragLine::drag(sheets[sheet], scenePos, style());
  LateNoteItem *note = newLateNote(sheet, scenePos, sp1);
  return note;
}

LateNoteItem *EntryScene::newLateNote(int sheet,
				      QPointF scenePos1, QPointF scenePos2) {
  if (scenePos2.isNull())
    scenePos2 = scenePos1;
  LateNoteData *data = data_.lateNoteManager()->newNote(scenePos1, scenePos2);
  ASSERT(data);
  data->setSheet(sheet);
  LateNoteItem *item = new LateNoteItem(data, lateNoteParent);
  sheets[sheet]->addItem(item); // ?
  item->makeWritable();
  item->setFocus();
  return item;  
}

void EntryScene::clipPgNoAt(int n) {
  firstDisallowedPgNo = n;
}

int EntryScene::clippedPgNo(int n) const {
  if (firstDisallowedPgNo>0 && n>=firstDisallowedPgNo)
    return firstDisallowedPgNo - 1;
  else
    return n;
}

QString EntryScene::pgNoToString(int n) const {
  int n0 = clippedPgNo(n);
  if (n0 == n)
    return QString::number(n);
  else if (n-n0<=26)
    return QString("%1%2").arg(n0).arg(QChar('a'+n-n0-1));
  else
    return QString("%1??").arg(n0);
}

QList<BlockItem const *> EntryScene::blocks() const {
  QList<BlockItem const *> bb;
  foreach (BlockItem *b, blockItems)
    bb << b;
  return bb;
}

QList<FootnoteItem const *> EntryScene::footnotes() const {
  QList<FootnoteItem const *> ff;
  foreach (BlockItem *bi, blockItems)
    foreach (FootnoteItem *f, bi->footnotes())
      ff << f;
  return ff;
}

void EntryScene::gotoSheetOfBlock(int n) {
  if (n<0) {
    emit sheetRequest(0);
  } else if (n>=blockItems.size()) {
    emit sheetRequest(nSheets-1);
  } else {
    emit sheetRequest(blockItems[n]->data()->sheet());
  }
}

void EntryScene::makeUnicellular(TableData *td) {
  int iblock = findBlock(td);
  if (iblock<0)
    return;

  TableBlockItem *tbi0 = dynamic_cast<TableBlockItem *>(blockItems[iblock]);
  ASSERT(tbi0);
  TextBlockData *tbd1 = tbi0->data()->deepCopyAsTextBlock();

  deleteBlock(iblock);
  TextBlockItem *tbi1 = injectTextBlock(tbd1, iblock);

  restackBlocks(iblock);
  gotoSheetOfBlock(iblock);
  tbi1->setFocus();
  TextCursor c = tbi1->textCursor();
  c.movePosition(TextCursor::End);
  tbi1->setTextCursor(c);
}
  
TableBlockItem *EntryScene::makeMulticellular(int pos, TextData *td) {
  int iblock = findBlock(td);
  if (iblock<0)
    return 0;

  TextBlockItem *tbi0 = dynamic_cast<TextBlockItem *>(blockItems[iblock]);
  ASSERT(tbi0);

  TableBlockData *tbd1 = TableBlockData::deepCopyFromTextBlock(tbi0->data(),
							       pos);
  ASSERT(tbd1);

  deleteBlock(iblock);
  TableBlockItem *tbi1 = injectTableBlock(tbd1, iblock);

  TextCursor c1(tbi1->document());
  c1.setPosition(pos+2);
  restackBlocks(iblock);
  gotoSheetOfBlock(iblock);
  tbi1->setFocus();
  tbi1->setTextCursor(c1);
  return tbi1;
}

void EntryScene::makeMulticellularAndPaste(TextData *td, QString txt) {
  qDebug() <<"mmapaste";
  TableBlockItem *tbi = makeMulticellular(0, td);
  if (!tbi)
    return; // oh well
  TextCursor c1(tbi->document());
  c1.setPosition(1);
  tbi->setTextCursor(c1);
  tbi->table()->pasteMultiCell(txt);
}

int EntryScene::findLastBlockOnSheet(int sh) {
  int B = blockItems.size();
  int res = -1;
  for (int b=0; b<B; b++) {
    int s = blockItems[b]->data()->sheet();
    if (s>sh)
      return res;
    else if (s==sh)
      res = b;
  }
  return res;
}
