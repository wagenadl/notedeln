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

#include "EntryScene.H"
#include "Style.H"
#include "BlockData.H"
#include "BlockItem.H"
#include "TitleItem.H"
#include "TextBlockItem.H"
#include "TableBlockItem.H"
#include "TextBlockData.H"
#include "TableBlockData.H"
#include "EntryData.H"
#include "GfxBlockItem.H"
#include "GfxBlockData.H"
#include "ResManager.H"
#include "Mode.H"
#include "FootnoteGroupItem.H"
#include "FootnoteData.H"
#include "FootnoteItem.H"
#include "Assert.H"
#include "Notebook.H"
#include "LateNoteItem.H"
#include "GfxNoteItem.H"
#include "GfxNoteData.H"
#include "TableItem.H"
#include "TextItemText.H"
#include "BelowItem.H"
#include "SvgFile.H"
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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>

EntryScene::EntryScene(EntryData *data, QObject *parent):
  BaseScene(data, parent),
  data_(data) {
  writable = false;
  firstDisallowedPgNo = 0;
  
  dateItem = 0;
  unlockedItem = 0;
  belowItem = 0;
  titleItemX = 0;

  vChangeMapper = new QSignalMapper(this);
  noteVChangeMapper = new QSignalMapper(this);
  futileMovementMapper = new QSignalMapper(this);
  connect(vChangeMapper, SIGNAL(mapped(int)), SLOT(vChanged(int)));
  connect(noteVChangeMapper, SIGNAL(mapped(int)), SLOT(noteVChanged(int)));
  connect(futileMovementMapper, SIGNAL(mapped(int)), SLOT(futileMovement(int)));
}

void EntryScene::populate() {
  BaseScene::populate();
  makeDateItem();
  makeBlockItems();
  positionNofNAndDateItems();
  positionTitleItem();
  positionBlocks();
  iSheet = -1; // cheat to force signal
  gotoSheet(0);

  if (data()->isUnlocked())
    addUnlockedWarning();
}

void EntryScene::makeBackground() {
  BaseScene::makeBackground();
  belowItem = new BelowItem();
  addItem(belowItem);
  belowItem->setRect(style_->real("margin-left"),
		     style_->real("margin-top"),
		     style_->real("page-width")
		     - style_->real("margin-left")
		     - style_->real("margin-right"),
		     style_->real("page-height")
		     - style_->real("margin-top"));
  belowItem->setPen(QPen(Qt::NoPen));
  belowItem->setBrush(QBrush(Qt::NoBrush));
}

void EntryScene::makeDateItem() {
  dateItem = addText(data_->created().toString(style().string("date-format")),
		     style().font("date-font"));
  dateItem->setDefaultTextColor(style().color("date-color"));
}

void EntryScene::makeTitleItem() {
  titleItemX = new TitleItem(data_->title(), 0);
  titleItem = titleItemX;
  connect(titleItemX,
	  SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	  SLOT(futileTitleMovement(int, Qt::KeyboardModifiers)));
  addItem(titleItem);
  titleItemX->makeWritableNoRecurse();

  connect(titleItemX->document(), SIGNAL(contentsChanged()),
	  SLOT(titleEdited()));
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
    FootnoteGroupItem *fng = new FootnoteGroupItem(bd, this);
    foreach (FootnoteItem *fni, fng->children<FootnoteItem>())
      connect(fni, SIGNAL(futileMovement()), SLOT(futileNoteMovement()));
    footnoteGroups.append(fng);
    connect(fng, SIGNAL(heightChanged()), noteVChangeMapper, SLOT(map()));    
  }
  redateBlocks();
  remap();
}

BlockItem *EntryScene::tryMakeGfxBlock(BlockData *bd) {
  GfxBlockData *gbd = dynamic_cast<GfxBlockData*>(bd);
  if (!gbd)
    return 0;
  GfxBlockItem *gbi = new GfxBlockItem(gbd);
  if (gbd->height()==0)
    gbi->sizeToFit();
  addItem(gbi);
  return gbi;
}

BlockItem *EntryScene::tryMakeTableBlock(BlockData *bd) {
  TableBlockData *tbd = dynamic_cast<TableBlockData*>(bd);
  if (!tbd)
    return 0;
  TableBlockItem *tbi = new TableBlockItem(tbd);
  if (tbd->height()==0)
    tbi->sizeToFit();
  addItem(tbi);
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  return tbi;
}

BlockItem *EntryScene::tryMakeTextBlock(BlockData *bd) {
  TextBlockData *tbd = dynamic_cast<TextBlockData*>(bd);
  if (!tbd)
    return 0;
  TextBlockItem *tbi = new TextBlockItem(tbd);
  if (tbd->height()==0)
    tbi->sizeToFit();
  addItem(tbi);
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  return tbi;
}
  
EntryScene::~EntryScene() {
}

void EntryScene::titleEdited() {
  positionTitleItem();
}

void EntryScene::positionTitleItem() {
  /* This keeps the title bottom aligned */
  if (!dateItem || !nOfNItem)
    return; // too early in process

  double dateX = dateItem->mapToScene(dateItem->boundingRect().topLeft()).x();
  titleItemX->setTextWidth(dateX - style().real("margin-left")
			   - style().real("title-indent") - 5);
  //  BaseScene::positionTitleItem();
  QPointF bl = titleItemX->netBounds().bottomLeft();
  titleItemX->setPos(style_->real("margin-left") -
		    bl.x() + style_->real("title-indent"),
		    style_->real("margin-top") -
		    style_->real("title-sep") -
		    bl.y());

  foreach (LateNoteItem *lni, titleItemX->children<LateNoteItem>())
    lni->setScale(1); // bizarre way to get date item in right spot
}

void EntryScene::positionNofNAndDateItems() {
  if (nSheets>1) {
    QPointF br = nOfNItem->boundingRect().bottomRight();
    nOfNItem->setPos(style().real("page-width") -
		     style().real("margin-right-over") -
		     br.x(),
		     style().real("margin-top") -
		     style().real("title-sep") -
		     br.y() + 8);
    QPointF tr = nOfNItem->sceneBoundingRect().topRight();
    br = dateItem->boundingRect().bottomRight();
    dateItem->setPos(tr - br + QPointF(0, 8));
  } else {
    QPointF br = dateItem->boundingRect().bottomRight();
    dateItem->setPos(style().real("page-width") -
		     style().real("margin-right-over") -
		     br.x(),
		     style().real("margin-top") -
		     style().real("title-sep") -
		     br.y());
  }
}

void EntryScene::positionBlocks() {
  int isheet = 0;
  foreach (BlockItem *bi, blockItems) {
    bi->resetPosition();
    isheet = bi->data()->sheet();
  }
  bool mustRestack = false;
  foreach (FootnoteGroupItem *fngi, footnoteGroups)
    if (fngi->resetPosition())
      mustRestack = true;
  nSheets = isheet+1;
  if (mustRestack) 
    for (int i=0; i<nSheets; i++)
      restackNotes(i);
}

void EntryScene::restackBlocks() {
  if (!writable)
    return;

  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double yblock = y0; // top of next block
  double yfng = y1; // bottom of next footnotegroup
  int sheet = 0; // sheet for next block & notegroup

  for (int i=0; i<blockItems.size(); ++i) {
    BlockItem *bi = blockItems[i];
    FootnoteGroupItem *fngi = footnoteGroups[i];
    double blockh = bi->data()->height();
    double fngh = fngi->netHeight();
    if (yblock+blockh > yfng-fngh && yblock > y0) {
      restackNotes(sheet); 
      sheet++;
      yblock = y0;
      yfng = y1;
    }
    if (bi->data()->y0() != yblock)
      bi->data()->setY0(yblock);
    if (bi->data()->sheet() != sheet)
      bi->data()->setSheet(sheet);
    bi->resetPosition();
    yblock += blockh;
    yfng -= fngh;
  }
  restackNotes(sheet);
  redateBlocks();
  nSheets = sheet + 1;
}

void EntryScene::redateBlocks() {
  QDate cre = data()->created().date();
  QDate mod = cre;
  QMap<BlockItem *, QString> txt;
  for (int i=0; i<blockItems.size(); i++) {
    QDate cre1 = blockItems[i]->data()->created().date();
    QDate mod1 = blockItems[i]->data()->modified().date();
    if (cre1==cre && mod1==mod)
      continue;
    QString txt1;
    if (cre1.year()==cre.year())
      txt1 = cre1.toString(style().string("date-format-yearless"));
    else
      txt1 = cre1.toString(style().string("date-format"));
    if (mod1!=cre1) {
      // date range
      txt1 += QString::fromUtf8("‒"); // figure dash
      if (mod1.year()==cre1.year())
	txt1 += mod1.toString(style().string("date-format-yearless"));
      else
	txt1 += mod1.toString(style().string("date-format"));
    }
    txt[blockItems[i]] = txt1;
    cre = cre1;
    mod = mod1;
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
      dateItem->setFont(style().font("date-font"));
      dateItem->setDefaultTextColor(style().color("date-color"));
    }
    dateItem->setPlainText(txt[i]);
    QPointF sp = i->scenePos();
    QRectF br = dateItem->sceneBoundingRect();
    double ml = style().real("margin-left");
    dateItem->setPos(QPointF(ml - br.width() - 2 - sp.x(),
			     style().real("text-block-above")));
  }    
}

void EntryScene::restackNotes(int sheet) {
  double yfng = style().real("page-height") - style().real("margin-bottom");
  for (int i=blockItems.size()-1; i>=0; --i) {
    BlockItem *bi = blockItems[i];
    if (bi->data()->sheet()==sheet) {
      FootnoteGroupItem *fngi = footnoteGroups[i];
      yfng -= fngi->netHeight();
      fngi->moveTo(yfng);
    }
  }
}

void EntryScene::gotoSheet(int i) {
  int oldSheet = iSheet; // keep info on old sheet
  
  BaseScene::gotoSheet(i);

  // Set visibility for all blocks
  int nBlocks = blockItems.size();
  for (int k=0; k<nBlocks; k++) {
    int s = blockItems[k]->data()->sheet();
    blockItems[k]->setVisible(s==iSheet);
    footnoteGroups[k]->setVisible(s==iSheet);
  }

  positionNofNAndDateItems();
  reshapeBelowItem();
  repositionContItem();

  // Set visibility for title-attached notes
  foreach (GfxNoteItem *gni, titleItemX->children<GfxNoteItem>()) 
    gni->setVisible(gni->data()->sheet()==iSheet);

  if (oldSheet!=iSheet) 
    emit nowOnPage(clippedPgNo(startPage()+iSheet));
}

void EntryScene::repositionContItem() {
  // just leave the thing in place
}

void EntryScene::reshapeBelowItem() {
  int iLast = findLastBlockOnSheet(iSheet);
  double ytop = iLast<0
    ? style().real("margin-top")
    :  blockItems[iLast]->sceneBoundingRect().bottom();
  belowItem->setRect(style().real("margin-left"),
		     ytop,
		     style().real("page-width")
		     - style().real("margin-left")
		     - style().real("margin-right"),
		     style().real("page-height")
		     - ytop);
}

int EntryScene::findLastBlockOnSheet(int sheet) {
  for (int i=blockItems.size()-1; i>=0; --i)
    if (blockItems[i]->data()->sheet() == sheet)
      return i;
  return -1;
}

bool EntryScene::belowContent(QPointF sp) {
  return itemAt(sp) == belowItem;
  //int iAbove = findLastBlockOnSheet(iSheet);
  //if (iAbove<0)
  //  return true;
  //else
  //  return sp.y() >= blockItems[iAbove]->netSceneRect().bottom();
}

void EntryScene::notifyChildless(BlockItem *gbi) {
  if (!dynamic_cast<GfxBlockItem*>(gbi))
    return;
  if (!writable)
    return;
  //  qDebug() << "childless" << this << gbi;
  for (int i=0; i<blockItems.size(); ++i) {
    if (blockItems[i] == gbi) {
      //      qDebug() << "deleting block " << i;
      deleteBlock(i);
      break;
    }
  }
  //  qDebug() << "restacking";
  restackBlocks();

  gotoSheet(iSheet>=nSheets ? nSheets-1 : iSheet);
  //  qDebug() << "all done" << iSheet << nSheets;
}
  
void EntryScene::deleteBlock(int blocki) {
  if (blocki>=blockItems.size()) {
    qDebug() << "EntryScene: deleting nonexisting block " << blocki;
    return;
  }
  BlockItem *bi = blockItems[blocki];
  BlockData *bd = bi->data();
  FootnoteGroupItem *fng = footnoteGroups[blocki];

  blockItems.removeAt(blocki);
  footnoteGroups.removeAt(blocki);
  remap();

  removeItem(bi);
  bi->deleteLater();
  data_->deleteBlock(bd);
  removeItem(fng);
  fng->deleteLater();
}

GfxBlockItem *EntryScene::newGfxBlock(int iAbove) {
  if (iAbove<0)
    iAbove = findLastBlockOnSheet(iSheet);
  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();
  //double yt = (iAbove>=0)
  //  ? blockItems[iAbove]->sceneBoundingRect().bottom()
  //  : style().real("margin-top");

  if (iAbove>=0) {
    // perhaps not create a new one after all
    GfxBlockItem *tbi = dynamic_cast<GfxBlockItem *>(blockItems[iAbove]);
    if (tbi && tbi->isWritable()) {
      // Previous block is writable, use it instead
      return tbi;
    }
  }

  GfxBlockData *gbd = new GfxBlockData();
  QList<BlockData *> existingBlocks = data_->blocks();
  if (iNew<existingBlocks.size())
    data_->insertBlockBefore(gbd, existingBlocks[iNew]);
  else
    data_->addBlock(gbd);
  GfxBlockItem *gbi = new GfxBlockItem(gbd);
  gbi->sizeToFit();
  addItem(gbi);
  gbi->makeWritable();
  FootnoteGroupItem *fng =  new FootnoteGroupItem(gbd, this);
  
  blockItems.insert(iNew, gbi);
  footnoteGroups.insert(iNew, fng);

  connect(gbi, SIGNAL(heightChanged()), vChangeMapper, SLOT(map()));
  connect(fng, SIGNAL(heightChanged()), noteVChangeMapper, SLOT(map()));    
  remap();

  restackBlocks();
  gotoSheetOfBlock(iNew);
  return gbi;
}

void EntryScene::gotoSheetOfBlock(int blocki) {
  ASSERT(blocki>=0 && blocki<blockItems.size());
  gotoSheet(blockItems[blocki]->data()->sheet());
}

void EntryScene::splitTextBlock(int iblock, int pos) {
  // block number iblock is going to be split
  TextBlockData *orig =
    dynamic_cast<TextBlockData*>(blockItems[iblock]->data());
  ASSERT(orig);
  TextBlockData *block1 = Data::deepCopy(orig);
  deleteBlock(iblock);
  TextBlockData *block2 = block1->split(pos);
  injectTextBlock(block1, iblock);
  TextBlockItem *tbi_post = injectTextBlock(block2, iblock+1);
  restackBlocks();
  gotoSheetOfBlock(iblock+1);
  tbi_post->setFocus();
}

void EntryScene::joinTextBlocks(int iblock_pre, int iblock_post) {
  ASSERT(iblock_pre<iblock_post);
  ASSERT(iblock_pre>=0);
  ASSERT(iblock_post<blockItems.size());
  TextBlockItem *tbi_pre
    = dynamic_cast<TextBlockItem*>(blockItems[iblock_pre]);
  TextBlockItem *tbi_post
    = dynamic_cast<TextBlockItem*>(blockItems[iblock_post]);
  ASSERT(tbi_pre);
  ASSERT(tbi_post);
  TextBlockData *block1 = Data::deepCopy(tbi_pre->data());
  TextBlockData *block2 = Data::deepCopy(tbi_post->data());
  int pos = block1->text()->text().size();
  deleteBlock(iblock_post);
  deleteBlock(iblock_pre);
  block1->join(block2);
  TextBlockItem *tbi = injectTextBlock(block1, iblock_pre);
  restackBlocks();
  gotoSheetOfBlock(iblock_pre);
  tbi->setFocus();
  QTextCursor c(tbi->text()->document());
  c.setPosition(pos);
  tbi->text()->setTextCursor(c);
}  

TableBlockItem *EntryScene::injectTableBlock(TableBlockData *tbd, int iblock) {
  // creates a new table block immediately before iblock (or at end if iblock
  // points past the last text block)
  BlockData *tbd_next =  iblock<blockItems.size()
    ? data_->blocks()[iblock]
    : 0;
  data_->insertBlockBefore(tbd, tbd_next);
  TableBlockItem *tbi = new TableBlockItem(tbd);
  tbi->sizeToFit();
  addItem(tbi);
  tbi->makeWritable();

  blockItems.insert(iblock, tbi);
  FootnoteGroupItem *fng = new FootnoteGroupItem(tbd, this);
  footnoteGroups.insert(iblock, fng);
  fng->makeWritable();
  connect(tbi, SIGNAL(heightChanged()), vChangeMapper, SLOT(map()));
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
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
  tbi->sizeToFit();
  addItem(tbi);
  tbi->makeWritable();

  blockItems.insert(iblock, tbi);
  FootnoteGroupItem *fng = new FootnoteGroupItem(tbd, this);
  footnoteGroups.insert(iblock, fng);
  fng->makeWritable();
  connect(tbi, SIGNAL(heightChanged()), vChangeMapper, SLOT(map()));
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  remap();
  return tbi;
}

void EntryScene::remap() {
  for (int i=0; i<blockItems.size(); i++) {
    BlockItem *bi = blockItems[i];
    vChangeMapper->setMapping(bi, i);
    futileMovementMapper->setMapping(bi, i);
    noteVChangeMapper->setMapping(footnoteGroups[i], i);
  }
}

TableBlockItem *EntryScene::newTableBlock(int iAbove) {
  if (iAbove<0)
    iAbove = findLastBlockOnSheet(iSheet);

  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();

  TableBlockData *tbd = new TableBlockData();
  TableBlockItem *tbi = injectTableBlock(tbd, iNew);
  
  restackBlocks();
  gotoSheetOfBlock(iNew);
  tbi->setFocus();
  return tbi;
}

TextBlockItem *EntryScene::newTextBlock(int iAbove, bool evenIfLastEmpty) {
  if (iAbove<0)
    iAbove = findLastBlockOnSheet(iSheet);

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
  
  restackBlocks();
  gotoSheetOfBlock(iNew);
  tbi->setFocus();
  return tbi;
}

void EntryScene::futileMovement(int block) {
  qDebug() << "futilemovement" << block;
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
    QTextCursor c = tbi->text()->textCursor();
    if (c.atEnd()) 
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
      if (bi->data()->type()=="textblock" && bi->isWritable()) {
	tgtidx = b;
	break;
      }
    }
  } else if (fmi.key()==Qt::Key_Right || fmi.key()==Qt::Key_Down
	     || fmi.key()==Qt::Key_Delete) {
    // downward movement
    for (int b=block+1; b<blockItems.size(); b++) {
      BlockItem *bi = blockItems[b];
      if (bi->data()->type()=="textblock" && bi->isWritable()) {
	tgtidx = b;
	break;
      }
    }
  }
  if (tgtidx<0) {
    // no target, go to start/end of current
    QTextCursor c = tbi->text()->textCursor();
    if (fmi.key()==Qt::Key_Down) {
      c.movePosition(QTextCursor::End);
      tbi->text()->setTextCursor(c);
    } else if (fmi.key()==Qt::Key_Up) {
      c.movePosition(QTextCursor::Start);
      tbi->text()->setTextCursor(c);
    }
    return;
  }

  if (fmi.key()==Qt::Key_Delete) {
    if (tgtidx==block+1) // do not combine across (e.g.) gfxblocks
      joinTextBlocks(block, tgtidx);
    return;
  } else if (fmi.key()==Qt::Key_Backspace) {
    if (tgtidx==block-1)
      joinTextBlocks(tgtidx, block);
    return;
  }
  
  TextBlockItem *tgt = dynamic_cast<TextBlockItem*>(blockItems[tgtidx]);
  ASSERT(tgt);
  if (tgt->data()->sheet()!=iSheet)
    gotoSheetOfBlock(tgtidx);
  tgt->setFocus();
  QTextDocument *doc = tgt->document();
  QTextCursor c(doc);
  QPointF p = tgt->text()->mapFromParent(tgt->mapFromScene(fmi.scenePos()));
  switch (fmi.key()) {
  case Qt::Key_Left: 
    c.movePosition(QTextCursor::End);
    break;
  case Qt::Key_Up: {
    QTextBlock blk = doc->lastBlock();
    QTextLayout *lay = blk.layout();
    QTextLine l = lay->lineAt(lay->lineCount()-1);
    c.setPosition(blk.position() + l.xToCursor(p.x() - lay->position().x()));
    break; }
  case Qt::Key_Right:
    c.movePosition(QTextCursor::Start);
    break;
  case Qt::Key_Down: {
    QTextBlock blk = doc->firstBlock();
    QTextLayout *lay = blk.layout();
    QTextLine l = lay->lineAt(0);
    c.setPosition(l.xToCursor(p.x() - blk.layout()->position().x()));
    break; }
  }
  tgt->text()->setTextCursor(c);
}

void EntryScene::futileTitleMovement(int key, Qt::KeyboardModifiers) {
  switch (key) {
  case Qt::Key_Enter: case Qt::Key_Return:
  case Qt::Key_Down:
    focusEnd();
    break;
  default:
    break;
  }
}

void EntryScene::focusEnd() {
  qDebug() << "EntryScene::focusEnd" << writable;
  if (!writable)
    return;

  TextBlockItem *lastbi = 0;
  for (int i=0; i<blockItems.size(); ++i) {
    TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[i]);
    if (tbi && tbi->data()->sheet()==iSheet)
      lastbi = tbi;
  }
  if (lastbi) {
    lastbi->setFocus();
    QTextCursor tc = lastbi->text()->textCursor();
    tc.movePosition(QTextCursor::End);
    lastbi->text()->setTextCursor(tc);    
  } else {
    if (iSheet == nSheets-1)
      newTextBlock(); // create new text block only on last sheet
  }
}


void EntryScene::vChanged(int block) {
  restackBlocks();
  gotoSheetOfBlock(block);
}

void EntryScene::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  // qDebug() << "EntryScene::mousePressEvent";
  QPointF sp = e->scenePos();
  bool take = false;
  if (inMargin(sp) && itemAt(sp)==bgItem) {
    //qDebug() << "  in margin";
    if (data_->book()->mode()->mode()==Mode::Annotate) {
      GfxNoteItem *note = createNote(sp);
      qDebug() << "created note" << note << iSheet;
      take = true;
    }
  } else if (belowContent(sp)) {
    //qDebug() << "  below content";
    switch (data_->book()->mode()->mode()) {
    case Mode::Mark: case Mode::Freehand:
      if (isWritable()) {
	GfxBlockItem *blk = newGfxBlock();
	e->setPos(blk->mapFromScene(e->scenePos())); // brutal!
	blk->mousePressEvent(e);
	take = true;
      }
      break;
    case Mode::Type:
      if (writable) {
	newTextBlock();
	take = true;
      }
      break;
    case Mode::Table:
      if (writable) {
	newTableBlock();
	take = true;
      }
      break;
    case Mode::Annotate: {
      createNote(sp);
      take = true;
    } break;
    default:
      break;
    }
  }
  if (take)
    e->accept();
  else
    QGraphicsScene::mousePressEvent(e);
}

void EntryScene::keyPressEvent(QKeyEvent *e) {
  if (e->key()==Qt::Key_Tab || e->key()==Qt::Key_Backtab) {
    TextItemText *focus = dynamic_cast<TextItemText*>(focusItem());
    if (focus) {
      focus->keyPressEvent(e);
      e->accept();
      return;
    }
  }
  if (e->modifiers() & Qt::ControlModifier) {
    bool steal = false;
    switch (e->key()) {
    case Qt::Key_V:
      if (writable)
	steal = tryToPaste();
      break;
    case Qt::Key_U:
      if (e->modifiers() & Qt::ShiftModifier) {
        unlock();
        steal = true;
      }
    }
    if (steal) {
      e->accept();
      return;
    }
  }
  BaseScene::keyPressEvent(e);
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
  unlockedItem = new QGraphicsTextItem();
  addItem(unlockedItem);
  unlockedItem->setPlainText(style().string("unlocked-text"));
  unlockedItem->setFont(style().font("unlocked-font"));
  unlockedItem->setDefaultTextColor(style().color("unlocked-color"));
  QRectF br = unlockedItem->sceneBoundingRect();
  unlockedItem->setPos(style().real("page-width") - br.width() - 4,
                       4);
}  


int EntryScene::findBlock(Item const *i) const {
  BlockItem const *bi = i->ancestralBlock();
  for (int i=0; i<blockItems.size(); i++) 
    if (blockItems[i] == bi)
      return i;
  return -1;
}

int EntryScene::findBlock(QPointF scenepos) const {
  for (int i=0; i<blockItems.size(); i++) {
    BlockData const *bd = blockItems[i]->data();
    if (bd->sheet() == iSheet && bd->y0()<=scenepos.y()
	&& bd->y0()+bd->height()>=scenepos.y())
      return i;
  }
  return -1;
}

/*
int EntryScene::findBlock(QPointF scenepos) const {
  Item *item;
  for (QGraphicsItem *gi = itemAt(scenepos); gi!=0; gi = gi->parentItem()) {
    item = dynamic_cast<Item *>(gi);
    if (item)
      return findBlock(item);
  }
  return -1;
}
*/

bool EntryScene::tryToPaste() {
  /* We get it first.
     If we don't send the event on to QGraphicsScene, textItems don't get it.
     What we do is dependent both on mimetype and on whether we have focus.
     If we have focus, text is not handled here, and images are placed in
     a new block below the focused block or in the focused block if it is a
     canvas.
     If we don't have focus, anything is placed in the focused block if it
     is a canvas, or in a new block if not.
  */
  if (!isWritable())
    return false;
  
  QPointF scenePos;
  QGraphicsTextItem *fi = dynamic_cast<QGraphicsTextItem*>(focusItem());

  if (fi) {
    scenePos = fi->mapToScene(posToPoint(fi, fi->textCursor().position()));
  } else {
    QList<QGraphicsView*> vv = views();
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
  
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  qDebug() << "EntryScene::trytopaste" << md;
  if (md->hasImage())
    return importDroppedImage(scenePos,
			      qvariant_cast<QImage>(md->imageData()),
			      QUrl());
  if (fi) 
    return false; // we'll import Urls as text into the focused item

  if (md->hasUrls())
    return importDroppedUrls(scenePos, md->urls());
  else if (md->hasText())
    return importDroppedText(scenePos, md->text());
  else
    return false;
}

bool EntryScene::dropBelow(QPointF scenePos, QMimeData const *md) {
  if (!isWritable())
    return false;
  if (md->hasImage())
    return importDroppedImage(scenePos,
			      qvariant_cast<QImage>(md->imageData()),
			      QUrl());
  else if (md->hasUrls())
    return importDroppedUrls(scenePos, md->urls());
  else if (md->hasText())
    return importDroppedText(scenePos, md->text());
  else
    return false;
}

bool EntryScene::importDroppedSvg(QPointF scenePos, QUrl const &source) {
  QImage img(SvgFile::downloadAsImage(source));
  if (img.isNull()) 
    return importDroppedText(scenePos, source.toString());
  else
    return importDroppedImage(scenePos, img, source);
}

bool EntryScene::importDroppedImage(QPointF scenePos, QImage const &img,
				   QUrl const &source) {
  // Return true if we want it
  /* If dropped on an existing gfxblock, insert it there.
     If dropped on belowItem, insert after last block on page.
     If dropped on text block, insert after that text block.
     Before creating a new graphics block, consider whether there is
     a graphics block right after it.
   */
  QGraphicsItem *gi = itemAt(scenePos);
  BlockItem *dst = 0;
  while (gi && !dst) {
    dst = dynamic_cast<BlockItem *>(gi);
    gi = gi->parentItem();
  }
  GfxBlockItem *gdst = dynamic_cast<GfxBlockItem*>(dst);
  QPointF p;
  if (gdst) {
    p = gdst->mapFromScene(scenePos);
  } else {
    p = QPointF(0, 0);
    gdst = dst
      ? gfxBlockAfter(indexOfBlock(dst))
      : gfxBlockAfter(findLastBlockOnSheet(iSheet));
  } 
  gdst->newImage(img, source, p);
  gotoSheet(gdst->data()->sheet());
  return true;
}

GfxBlockItem *EntryScene::gfxBlockAfter(int iblock) {
  if (iblock>=0 && iblock+1<blockItems.size()) 
    if (dynamic_cast<GfxBlockItem*>(blockItems[iblock+1]))
      return dynamic_cast<GfxBlockItem*>(blockItems[iblock+1]);
  return newGfxBlock(iblock);
}

int EntryScene::indexOfBlock(BlockItem *bi) const {
  for (int i=0; i<blockItems.size(); ++i)
    if (blockItems[i]==bi)
      return i;
  return -1;
}

bool EntryScene::importDroppedUrls(QPointF scenePos, QList<QUrl> const &urls) {
  bool ok = false;
  foreach (QUrl const &u, urls)
    if (importDroppedUrl(scenePos, u))
      ok = true;
  return ok;
}

bool EntryScene::importDroppedUrl(QPointF scenePos, QUrl const &url) {
  // QGraphicsItem *dst = itemAt(scenePos);
  /* A URL could be any of the following:
     (1) A local image file
     (2) A local file of non-image type
     (3) An internet-located image file
     (4) A web-page
     (5) Anything else
  */
  qDebug() << "importdroppedurl" << url.toString();
  if (url.isLocalFile()) {
    QString path = url.toLocalFile();
    if (path.endsWith(".svg")) 
      return importDroppedSvg(scenePos, url);
    QImage image = QImage(path);
    if (!image.isNull())
      return importDroppedImage(scenePos, image, url);
    else 
      return importDroppedFile(scenePos, path);
  } else {
    // Right now, we import all network urls as text
    return importDroppedText(scenePos, url.toString());
  }
  return false;
}

bool EntryScene::importDroppedText(QPointF scenePos, QString const &txt) {
  TextItem *ti = 0;
  if (belowContent(scenePos)) {
    TextBlockItem *tbi = newTextBlock();
    if (!tbi)
      return false;
    ti = tbi->text();
  } else {
    int blk = findBlock(scenePos);
    if (blk<0 || !blockItems[blk]->isWritable())
      return false;
    GfxBlockItem *gbi = dynamic_cast<GfxBlockItem*>(blockItems[blk]);
    TextBlockItem *tbi = dynamic_cast<TextBlockItem*>(blockItems[blk]);
    if (gbi) {
      // let's create a new note at the target position
      GfxNoteItem *note = gbi->createNote(gbi->mapFromScene(scenePos), false);
      ti = note->textItem();
    } else if (tbi) {
      ti = tbi->text();
    }
  }
  if (!ti)
    return false;
  QTextCursor c = ti->textCursor();
  int pos = ti->pointToPos(ti->mapFromScene(scenePos));
  if (pos>=0)
    c.setPosition(pos);
  else
    c.clearSelection();
  c.insertText(txt);
  ti->setFocus();
  ti->setTextCursor(c);
  return true;
}

bool EntryScene::importDroppedFile(QPointF scenePos, QString const &fn) {
  //  qDebug() << "EntryScene: import dropped file: " << scenePos << fn;
  if (!fn.startsWith("/"))
    return false;
  TextItem *ti = 0;
  if (belowContent(scenePos)) {
    TextBlockItem *tbi = newTextBlock();
    if (!tbi)
      return false;
    ti = tbi->text();
  } else {
    int blk = findBlock(scenePos);
    if (!blk || !blockItems[blk]->isWritable())
      return false;
    GfxBlockItem *gbi = dynamic_cast<GfxBlockItem*>(blockItems[blk]);
    TextBlockItem *tbi = dynamic_cast<TextBlockItem*>(blockItems[blk]);
    if (gbi) {
      // let's create a new note at the target position
      GfxNoteItem *note = gbi->createNote(gbi->mapFromScene(scenePos), false);
      ti = note->textItem();
    } else if (tbi) {
      ti = tbi->text();
    }
  }
  if (!ti)
    return false;
  QTextCursor c = ti->textCursor();
  int pos = ti->pointToPos(ti->mapFromScene(scenePos));
  if (pos>=0)
    c.setPosition(pos);
  else
    c.clearSelection();
  int start = c.position();
  c.insertText(fn);
  int end = c.position();
  ti->addMarkup(MarkupData::Link, start, end);
  ti->setFocus();
  ti->setTextCursor(c);
  return true;
}
    
void EntryScene::makeWritable() {
  writable = true;
  belowItem->setCursor(Qt::IBeamCursor);
  bgItem->setAcceptDrops(true);
  titleItemX->makeWritable();
  foreach (BlockItem *bi, blockItems)
    bi->makeWritable();
  foreach (FootnoteGroupItem *fng, footnoteGroups)
    fng->makeWritable();
}

int EntryScene::startPage() const {
  return data_->startPage();
}

QString EntryScene::title() const {
  return data_->title()->current()->text();
}

bool EntryScene::isWritable() const {
  return writable;
}

void EntryScene::newFootnote(int block, QString tag) {
  ASSERT(block>=0 && block<blockItems.size());
  foreach (FootnoteItem *fni, footnoteGroups[block]->children<FootnoteItem>()) {
    //    qDebug() << "Comparing" << tag << " against " << fni->data()->tag();
    if (fni->data()->tag()==tag) {
      fni->setFocus();
      return;
    }
  }
  FootnoteData *fnd = new FootnoteData(blockItems[block]->data());
  fnd->setTag(tag);
  FootnoteItem *fni = new FootnoteItem(fnd, footnoteGroups[block]);
  connect(fni, SIGNAL(futileMovement()), SLOT(futileNoteMovement()));
  fni->makeWritable();
  fni->sizeToFit();
  restackBlocks();
  gotoSheetOfBlock(block);
  if (!fni->setAutoContents())
    fni->setFocus();
}

void EntryScene::noteVChanged(int block) {
  qDebug() << "noteVChanged" << block;
  restackBlocks();
  gotoSheetOfBlock(block);
}

void EntryScene::futileNoteMovement() {
  // not yet implemented
}

EntryData *EntryScene::data() const {
  return data_;
}

GfxNoteItem *EntryScene::createNote(QPointF scenePos) {
  GfxNoteItem *note = titleItemX->createNote(titleItem->mapFromScene(scenePos));
  if (note)
    note->data()->setSheet(iSheet);
  return note;
  
}

GfxNoteItem *EntryScene::newNote(QPointF scenePos1, QPointF scenePos2) {
  if (scenePos2.isNull())
    scenePos2 = scenePos1;
  GfxNoteItem *note = titleItemX->newNote(titleItem->mapFromScene(scenePos1),
                                          titleItem->mapFromScene(scenePos2));
  if (note)
    note->data()->setSheet(iSheet);
  return note;  
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
  else
    return QString("%1%2").arg(n0).arg(QChar('a'+n-n0-1));
}

QList<BlockItem const *> EntryScene::blocks() const {
  QList<BlockItem const *> bb;
  foreach (BlockItem *b, blockItems)
    bb << b;
  return bb;
}

QList<FootnoteItem const *> EntryScene::footnotes() const {
  QList<FootnoteItem const *> ff;
  foreach (FootnoteGroupItem *fg, footnoteGroups)
    foreach (FootnoteItem *f, fg->children<FootnoteItem>())
    ff << f;
  return ff;
}
