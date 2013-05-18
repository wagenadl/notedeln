// PageScene.C

#include "PageScene.H"
#include "Style.H"
#include "BlockData.H"
#include "BlockItem.H"
#include "TitleItem.H"
#include "TextBlockItem.H"
#include "TextBlockData.H"
#include "PageData.H"
#include "GfxBlockItem.H"
#include "GfxBlockData.H"
#include "ResManager.H"
#include "Mode.H"
#include "FootnoteGroupItem.H"
#include "FootnoteData.H"
#include "FootnoteItem.H"
#include "Assert.H"
#include "Notebook.H"
#include "GfxNoteItem.H"
#include "GfxNoteData.H"

#include <QTemporaryFile>
#include <QProcess>
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

PageScene::PageScene(PageData *data, QObject *parent):
  BaseScene(data, parent),
  data_(data) {
  writable = false;
  
  dateItem = 0;

  vChangeMapper = new QSignalMapper(this);
  noteVChangeMapper = new QSignalMapper(this);
  futileMovementMapper = new QSignalMapper(this);
  connect(vChangeMapper, SIGNAL(mapped(int)), SLOT(vChanged(int)));
  connect(noteVChangeMapper, SIGNAL(mapped(int)), SLOT(noteVChanged(int)));
  connect(futileMovementMapper, SIGNAL(mapped(int)), SLOT(futileMovement(int)));
}

void PageScene::populate() {
  BaseScene::populate();
  makeDateItem();
  makeBlockItems();
  positionNofNAndDateItems();
  positionTitleItem();
  stackBlocks();
  iSheet = -1; // cheat to force signal
  gotoSheet(0);
}

void PageScene::makeBackground() {
  BaseScene::makeBackground();
  belowItem = addRect(style_->real("margin-left"),
		      style_->real("margin-top"),
		      style_->real("page-width")
		      - style_->real("margin-left")
		      - style_->real("margin-right"),
		      style_->real("page-height")
		      - style_->real("margin-top")
		      - style_->real("margin-bottom"),
		      QPen(Qt::NoPen),
		      QBrush(Qt::NoBrush));
}

void PageScene::makeDateItem() {
  dateItem = addText(data_->created().toString(style().string("date-format")),
		     style().font("date-font"));
  dateItem->setDefaultTextColor(style().color("date-color"));
}

void PageScene::makeTitleItem() {
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

void PageScene::makeBlockItems() {
  foreach (BlockData *bd, data_->blocks()) {
    BlockItem *bi = tryMakeTextBlock(bd);
    if (!bi)
      bi = tryMakeGfxBlock(bd);
    ASSERT(bi);
    connect(bi, SIGNAL(boundsChanged()), vChangeMapper, SLOT(map()));
    blockItems.append(bi);
    sheetNos.append(bd->sheet());
    topY.append(bd->y0());
    FootnoteGroupItem *fng = new FootnoteGroupItem(bd, this);
    foreach (FootnoteItem *fni, fng->children<FootnoteItem>())
      connect(fni, SIGNAL(futileMovement()), SLOT(futileNoteMovement()));
    footnoteGroups.append(fng);
    connect(fng, SIGNAL(vChanged()), noteVChangeMapper, SLOT(map()));    
  }
  remap();
}

BlockItem *PageScene::tryMakeGfxBlock(BlockData *bd) {
  GfxBlockData *gbd = dynamic_cast<GfxBlockData*>(bd);
  if (!gbd)
    return 0;
  GfxBlockItem *gbi = new GfxBlockItem(gbd);
  addItem(gbi);
  return gbi;
}

BlockItem *PageScene::tryMakeTextBlock(BlockData *bd) {
  TextBlockData *tbd = dynamic_cast<TextBlockData*>(bd);
  if (!tbd)
    return 0;
  TextBlockItem *tbi = new TextBlockItem(tbd);
  addItem(tbi);
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  return tbi;
}
  
PageScene::~PageScene() {
}

void PageScene::titleEdited() {
  positionTitleItem();
}

void PageScene::positionTitleItem() {
  /* This keeps the title bottom aligned */
  if (!dateItem || !nOfNItem)
    return; // too early in process

  double dateX = dateItem->mapToScene(dateItem->boundingRect().topLeft()).x();
  titleItemX->setTextWidth(dateX - style().real("margin-left")
			   - style().real("title-sep") - 5);
  //  BaseScene::positionTitleItem();
  QPointF bl = titleItemX->netChildBoundingRect().bottomLeft();
  titleItemX->setPos(style_->real("margin-left") -
		    bl.x() + style_->real("title-sep"),
		    style_->real("margin-top") -
		    style_->real("title-sep") -
		    bl.y());

}

void PageScene::positionNofNAndDateItems() {
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

void PageScene::stackBlocks() {
  sheetNos.clear();
  topY.clear();
  double y0 = style().real("margin-top");
  for (int n=0; n<blockItems.size(); n++) {
    sheetNos.append(blockItems[n]->data()->sheet());
    topY.append(y0);
  }
  restackBlocks(0, true);
}

void PageScene::restackBlocks(int starti, bool preferData) {
  if (!writable)
    preferData = true;
  int endi = sheetNos.size();
  if (starti>=endi)
    return;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  if (starti<0) {
    starti=0;
    topY[starti] = y0;
  }
  double y = topY[starti];
  int sheet = sheetNos[starti];
  double y1a = y1;

  // we are assuming that the blockitems and footnotegroups are unscaled!
  for (int i=0; i<starti; i++)
    if (sheetNos[i]==sheet)
      y1a -= footnoteGroups[i]->netChildBoundingRect().height();

  for (int i=starti; i<endi; i++)
    sheetNos[i] = -1;    
  
  for (int i=starti; i<endi; i++) {
    BlockItem *bi = blockItems[i];
    BlockData *bd = bi->data();
    double h = bi->boundingRect().height();
    FootnoteGroupItem *fng = footnoteGroups[i];
    double fnh = fng->netChildBoundingRect().height();
    bool updateData = false;
    if (preferData && bd->sheet()>=0) {
      // (sheet,y) information stored in data, we'll use it
      if (bd->sheet()!=sheet) {
	restackFootnotes(sheet); // about to move away from sheet, so do it now
	sheet = bd->sheet();
      }
      y = bd->y0();
    } else {
      if (y>y0 && y+h>y1a - fnh) {
	restackFootnotes(sheet); // about to move away from sheet, so do it now
	y = y0;
	y1a = y1;
	sheet++;
      }
      updateData = true;
    }
    topY[i] = y;
    sheetNos[i] = sheet;
    bi->moveBy(0, y - bi->sceneBoundingRect().top());

    if (updateData) {
      BlockData *bd = bi->data();
      bd->setY0(y);
      bd->setSheet(sheet);
    }

    y = y + h;
  }
  restackFootnotes(sheet); // do last sheet in the entry
  nSheets = sheet + 1;
}

void PageScene::restackFootnotes(int sheet) {
  double accumh = 0;
  for (int k=0; k<footnoteGroups.size(); k++) 
    if (sheetNos[k] == sheet)
      accumh += footnoteGroups[k]->netChildBoundingRect().height();

  double y = style().real("page-height")
    - style().real("margin-bottom")
    - accumh;

  for (int k=0; k<footnoteGroups.size(); k++) {
    if (sheetNos[k] == sheet) {
      footnoteGroups[k]->setPos(style().real("margin-left"), y);
      y += footnoteGroups[k]->netChildBoundingRect().height();
    }
  }
}

void PageScene::gotoSheet(int i) {
  int oldSheet = iSheet; // keep info on old sheet
  
  BaseScene::gotoSheet(i);

  // Set visibility for all blocks
  int nBlocks = blockItems.size();
  for (int k=0; k<nBlocks; k++)
    blockItems[k]->setVisible(sheetNos[k]==iSheet);
  for (int k=0; k<nBlocks; k++)
    footnoteGroups[k]->setVisible(sheetNos[k]==iSheet);

  positionNofNAndDateItems();
  reshapeBelowItem();
  repositionContItem();

  // Set visibility for title-attached notes
  foreach (GfxNoteItem *gni, titleItemX->children<GfxNoteItem>()) 
    gni->setVisible(gni->data()->sheet()==iSheet);
  
  if (oldSheet!=iSheet)
    emit nowOnPage(startPage()+iSheet);
}

void PageScene::repositionContItem() {
  // just leave the thing in place
}

void PageScene::reshapeBelowItem() {
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
		     - ytop
		     - style().real("margin-bottom"));
}

int PageScene::findLastBlockOnSheet(int sheet) {
  double maxY = 0;
  int iAbove = -1;
  for (int i=0; i<blockItems.size(); i++) {
    if (sheetNos[i] == sheet) {
      double by = blockItems[i]->sceneBoundingRect().bottom();
      if (by>maxY) {
	maxY = by;
	iAbove = i;
      }
    }
  }
  return iAbove;
}

bool PageScene::belowContent(QPointF sp) {
  return itemAt(sp) == belowItem;
  //int iAbove = findLastBlockOnSheet(iSheet);
  //if (iAbove<0)
  //  return true;
  //else
  //  return sp.y() >= blockItems[iAbove]->netSceneRect().bottom();
}

void PageScene::notifyChildless(BlockItem *gbi) {
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
  
void PageScene::deleteBlock(int blocki) {
  if (blocki>=blockItems.size()) {
    qDebug() << "PageScene: deleting nonexisting block " << blocki;
    return;
  }
  BlockItem *bi = blockItems[blocki];
  BlockData *bd = bi->data();
  FootnoteGroupItem *fng = footnoteGroups[blocki];

  blockItems.removeAt(blocki);
  sheetNos.removeAt(blocki);
  topY.removeAt(blocki);
  footnoteGroups.removeAt(blocki);
  remap();

  removeItem(bi);
  // qDebug() << "removed block from scene" << bi;
  bi->deleteLater();
  qDebug() << "queued for deletion: " << bi;
  //delete bi;
  data_->deleteBlock(bd);
  removeItem(fng);
  fng->deleteLater();
   // delete fng;
}

GfxBlockItem *PageScene::newGfxBlock(int iAbove) {
  if (iAbove<0)
    iAbove = findLastBlockOnSheet(iSheet);
  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();
  double yt = (iAbove>=0)
    ? blockItems[iAbove]->sceneBoundingRect().bottom()
    : style().real("margin-top");

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
  addItem(gbi);
  gbi->makeWritable();
  FootnoteGroupItem *fng =  new FootnoteGroupItem(gbd, this);
  
  blockItems.insert(iNew, gbi);
  sheetNos.insert(iNew, iSheet);
  topY.insert(iNew, yt);
  footnoteGroups.insert(iNew, fng);

  connect(gbi, SIGNAL(boundsChanged()), vChangeMapper, SLOT(map()));
  connect(fng, SIGNAL(vChanged()), noteVChangeMapper, SLOT(map()));    
  remap();

  restackBlocks(iNew);
  gotoSheet(sheetNos[iNew]);
  return gbi;
}

void PageScene::splitTextBlock(int iblock, int pos) {
  // block number iblock is going to be split
  TextBlockData *orig =
    dynamic_cast<TextBlockData*>(blockItems[iblock]->data());
  ASSERT(orig);
  TextBlockData *block1 = Data::deepCopy(orig);
  deleteBlock(iblock);
  TextBlockData *block2 = block1->split(pos);
  injectTextBlock(block1, iblock);
  TextBlockItem *tbi_post = injectTextBlock(block2, iblock+1);
  restackBlocks(iblock-1);
  gotoSheet(sheetNos[iblock+1]);
  tbi_post->setFocus();
}

void PageScene::joinTextBlocks(int iblock_pre, int iblock_post) {
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
  restackBlocks(iblock_pre-1);
  gotoSheet(sheetNos[iblock_pre]);
  tbi->setFocus();
  QTextCursor c(tbi->text()->document());
  c.setPosition(pos);
  tbi->text()->setTextCursor(c);
}  

TextBlockItem *PageScene::injectTextBlock(TextBlockData *tbd, int iblock) {
  // creates a new text block immediately before iblock (or at end if iblock
  // points past the last text block)
  BlockData *tbd_next =  iblock<blockItems.size()
    ? data_->blocks()[iblock]
    : 0;
  data_->insertBlockBefore(tbd, tbd_next);
  TextBlockItem *tbi = new TextBlockItem(tbd);
  addItem(tbi);
  tbi->makeWritable();

  blockItems.insert(iblock, tbi);
  sheetNos.insert(iblock, iSheet);
  topY.insert(iblock, 0);
  footnoteGroups.insert(iblock, new FootnoteGroupItem(tbd, this));
  connect(tbi, SIGNAL(boundsChanged()), vChangeMapper, SLOT(map()));
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  remap();
  return tbi;
}

void PageScene::remap() {
  for (int i=0; i<blockItems.size(); i++) {
    BlockItem *bi = blockItems[i];
    vChangeMapper->setMapping(bi, i);
    futileMovementMapper->setMapping(bi, i);
    noteVChangeMapper->setMapping(footnoteGroups[i], i);
  }
}

TextBlockItem *PageScene::newTextBlock(int iAbove, bool evenIfLastEmpty) {
  if (iAbove<0)
    iAbove = findLastBlockOnSheet(iSheet);

  if (iAbove>=0 && !evenIfLastEmpty) {
    TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[iAbove]);
    if (tbi && tbi->document()->isEmpty() && tbi->isWritable()) {
      // Previous block is writable empty text, go there instead
      tbi->setFocus();
      return tbi;
    }
  }      

  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();
  double yt = (iAbove>=0)
    ? blockItems[iAbove]->sceneBoundingRect().bottom()
    : style().real("margin-top");

  TextBlockData *tbd = new TextBlockData();
  TextBlockItem *tbi = injectTextBlock(tbd, iNew);
  topY[iNew] = yt;
  
  restackBlocks(iNew);
  gotoSheet(sheetNos[iNew]);
  tbi->setFocus();
  return tbi;
}

void PageScene::futileMovement(int block) {
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
      if (dynamic_cast<TextBlockItem *>(bi) && bi->isWritable()) {
	tgtidx = b;
	break;
      }
    }
  } else if (fmi.key()==Qt::Key_Right || fmi.key()==Qt::Key_Down
	     || fmi.key()==Qt::Key_Delete) {
    // downward movement
    for (int b=block+1; b<blockItems.size(); b++) {
      BlockItem *bi = blockItems[b];
      if (dynamic_cast<TextBlockItem *>(bi) && bi->isWritable()) {
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
  if (sheetNos[tgtidx]!=iSheet)
    gotoSheet(sheetNos[tgtidx]);
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
    c.setPosition(l.xToCursor(p.x() - blk.layout()->position().x()));
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

void PageScene::futileTitleMovement(int key, Qt::KeyboardModifiers) {
  switch (key) {
  case Qt::Key_Enter: case Qt::Key_Return:
  case Qt::Key_Down:
    focusEnd();
    break;
  default:
    break;
  }
}

void PageScene::focusEnd() {
  if (!writable)
    return;

  TextBlockItem *lastbi = 0;
  for (int i=0; i<blockItems.size(); ++i) {
    if (sheetNos[i] == iSheet) {
      TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[i]);
      if (tbi)
	lastbi = tbi;
    }
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


void PageScene::vChanged(int block) {
  ASSERT(block>=0 && block<blockItems.size());
  restackBlocks(block>0 ? block-1 : block);
  gotoSheet(sheetNos[block]);
}

void PageScene::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  // qDebug() << "PageScene::mousePressEvent";
  QPointF sp = e->scenePos();
  bool take = false;
  if (inMargin(sp) && itemAt(sp)==bgItem) {
    //qDebug() << "  in margin";
    if (data_->book()->mode()->mode()==Mode::Annotate) {
      GfxNoteItem *note = 
        titleItemX->createNote(titleItem->mapFromScene(sp),
                               !data()->isRecent());
      if (note)
        note->data()->setSheet(iSheet);
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
    case Mode::Annotate: {
      GfxNoteItem *note = 
        titleItemX->createNote(titleItem->mapFromScene(sp),
                               !data()->isRecent());
      if (note)
        note->data()->setSheet(iSheet);
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

void PageScene::keyPressEvent(QKeyEvent *e) {
  if (e->modifiers() & Qt::ControlModifier) {
    bool steal = false;
    switch (e->key()) {
    case Qt::Key_V:
      if (writable)
	steal = tryToPaste();
      break;
    }
    if (steal) {
      e->accept();
      return;
    }
  }
  BaseScene::keyPressEvent(e);
}

int PageScene::findBlock(Item const *i) const {
  BlockItem const *bi = i->ancestralBlock();
  for (int i=0; i<blockItems.size(); i++) 
    if (blockItems[i] == bi)
      return i;
  return -1;
}

int PageScene::findBlock(QPointF scenepos) const {
  Item *item;
  for (QGraphicsItem *gi = itemAt(scenepos); gi!=0; gi = gi->parentItem()) {
    item = dynamic_cast<Item *>(gi);
    if (item)
      return findBlock(item);
  }
  return -1;
}

bool PageScene::tryToPaste() {
  // we get it first.
  // if we don't send the event on to QGraphicsScene, textItems don't get it
  //  qDebug() << "PageScene::tryToPaste";

  QPointF scenePos;

  QGraphicsTextItem *fi = dynamic_cast<QGraphicsTextItem*>(focusItem());
  if (fi) {
    QPointF p = posToPoint(fi, fi->textCursor().position());
    qDebug() << "PageScene::tryToPaste: have focusItem " << fi << p;
    scenePos = fi->mapToScene(p);
  } else {
    QList<QGraphicsView*> vv = views();
    if (vv.isEmpty()) {
      qDebug() << "PageScene: cannot determine paste position: no view";
      return false;
    }
    if (vv.size()>1) {
      qDebug() << "PageScene: multiple views: cannot determine paste position";
      // Of course, this can actually be done just fine, but I haven't
      // figured it out yet.
      return false;
    }
    scenePos = vv[0]->mapToScene(vv[0]->mapFromGlobal(QCursor::pos()));
  }
  
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  qDebug() << "PageScene::trytopaste" << md;
  bool accept = importDroppedOrPasted(scenePos, md, false);
  return accept;
}

void PageScene::dropEvent(QGraphicsSceneDragDropEvent *e) {
  if (!writable) {
    e->ignore();
    return;
  }
  if (e->source() == 0) {
    // event from outside our application
    bool accept = importDroppedOrPasted(e->scenePos(), e->mimeData(), true);
    if (accept) {
      e->setDropAction(Qt::CopyAction);
      e->accept();
    } else {
      e->setDropAction(Qt::IgnoreAction);
      e->ignore(); // this does not seem to graphically refuse the offering
    }
  } else {
    // event from inside our application
    // we may be dragging some image around
    // not yet implemented
    //    qDebug() << "PageScene: internal drop";
    QGraphicsScene::dropEvent(e);
  }
}

bool PageScene::importDroppedOrPasted(QPointF scenePos,
				      QMimeData const *md,
				      bool dropped) {
  bool accept = false;
  if (md->hasImage()) 
    accept = importDroppedImage(scenePos,
				qvariant_cast<QImage>(md->imageData()),
				QUrl());
  else if (md->hasUrls()) 
    accept = importDroppedUrls(scenePos, md->urls(), dropped);
  else if (md->hasText())
    accept = importDroppedText(scenePos, md->text());
  return accept;
}

bool PageScene::importDroppedSvg(QPointF scenePos, QUrl const &source) {
  QTemporaryFile f(QDir::tempPath() + "/eln_XXXXXX.png");
  f.open(); // without this, no filename is generated
  QStringList args; args << "-l" << source.toString() << f.fileName();
  int res = QProcess::execute("webgrab", args);
  if (res==0) {
    // success
    QImage img(f.fileName());
    return importDroppedImage(scenePos, img, source);
  }
  qDebug() << "importDroppedSvg: failed";
  return importDroppedText(scenePos, source.toString());
}

bool PageScene::importDroppedImage(QPointF scenePos, QImage const &img,
				   QUrl const &source) {
  // Return true if we want it
  /* If dropped on an existing gfxblock, insert it there.
     If dropped on belowItem, insert after last block on page.
     If dropped on text block, insert after that text block.
     Before creating a new graphics block, consider whether there is
     a graphics block right after it.
   */
  QGraphicsItem *gi = itemAt(scenePos);
  BlockItem *dst;
  while (true) {
    dst = dynamic_cast<BlockItem *>(gi);
    if (dst)
      break;
    if (gi)
      gi = gi->parentItem();
    else
      break;
  }
  GfxBlockItem *gdst = dynamic_cast<GfxBlockItem*>(dst);
  if (dst) {
    gdst = dynamic_cast<GfxBlockItem*>(dst);
    if (!gdst)
      gdst = gfxBlockAfter(indexOfBlock(dst));
  } else {
    gdst = gfxBlockAfter(findLastBlockOnSheet(iSheet));
  } 
  gdst->newImage(img, source, gdst->mapFromScene(scenePos));
  int i = indexOfBlock(gdst);
  ASSERT(i>=0);
  gotoSheet(sheetNos[i]);
  return true;
}

GfxBlockItem *PageScene::gfxBlockAfter(int iblock) {
  if (iblock>=0 && iblock+1<blockItems.size()) 
    if (dynamic_cast<GfxBlockItem*>(blockItems[iblock+1]))
      return dynamic_cast<GfxBlockItem*>(blockItems[iblock+1]);
  return newGfxBlock(iblock);
}

int PageScene::indexOfBlock(BlockItem *bi) const {
  for (int i=0; i<blockItems.size(); ++i)
    if (blockItems[i]==bi)
      return i;
  return -1;
}

bool PageScene::importDroppedUrls(QPointF scenePos, QList<QUrl> const &urls,
				  bool dropped) {
  bool ok = false;
  foreach (QUrl const &u, urls)
    if (importDroppedUrl(scenePos, u, dropped))
      ok = true;
  return ok;
}

bool PageScene::importDroppedUrl(QPointF scenePos,
				 QUrl const &url,
				 bool /*dropped*/) {
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

bool PageScene::importDroppedText(QPointF scenePos, QString const &txt) {
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

bool PageScene::importDroppedFile(QPointF scenePos, QString const &fn) {
  //  qDebug() << "PageScene: import dropped file: " << scenePos << fn;
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
    
void PageScene::makeWritable() {
  writable = true;
  belowItem->setCursor(Qt::IBeamCursor);
  bgItem->setAcceptDrops(true);
  titleItemX->makeWritable();
  foreach (BlockItem *bi, blockItems)
    bi->makeWritable();
  foreach (FootnoteGroupItem *fng, footnoteGroups)
    fng->makeWritable();
}

int PageScene::startPage() const {
  return data_->startPage();
}

bool PageScene::isWritable() const {
  return writable;
}

void PageScene::newFootnote(int block, QString tag) {
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
  if (!fni->setAutoContents())
    fni->setFocus();
  footnoteGroups[block]->restack();
  restackBlocks(block);
}

void PageScene::noteVChanged(int block) {
  qDebug() << "noteVChanged" << block;
  ASSERT(block>=0 && block<blockItems.size());
  int s = sheetNos[block];
  for (int i=0; i<block; i++) {
    if (sheetNos[i]==s) {
      qDebug() << " -> restacking blocks from " << i;
      restackBlocks(i);
      return;
    }
  }
  qDebug() << " -> restacking blocks from " << block;
  restackBlocks(block);
}

void PageScene::futileNoteMovement() {
  // not yet implemented
}

PageData *PageScene::data() const {
  return data_;
}
