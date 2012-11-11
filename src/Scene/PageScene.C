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
#include "ResourceManager.H"
#include "ModSnooper.H"
#include "FootnoteGroupItem.H"
#include "FootnoteData.H"
#include "FootnoteItem.H"

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

PageScene::PageScene(PageData *data, QObject *parent):
  BaseScene(data, parent),
  data(data) {
  networkManager = 0;
  writable = false;
  
  hChangeMapper = new QSignalMapper(this);
  vChangeMapper = new QSignalMapper(this);
  noteVChangeMapper = new QSignalMapper(this);
  futileMovementMapper = new QSignalMapper(this);
  connect(hChangeMapper, SIGNAL(mapped(int)), SLOT(hChanged(int)));
  connect(vChangeMapper, SIGNAL(mapped(int)), SLOT(vChanged(int)));
  connect(noteVChangeMapper, SIGNAL(mapped(int)), SLOT(noteVChanged(int)));
  connect(futileMovementMapper, SIGNAL(mapped(int)), SLOT(futileMovement(int)));

  nOfNItem = 0;
  dateItem = 0;
}

void PageScene::populate() {
  BaseScene::populate();
  makeDateItem();
  positionTitleItem();
  makeBlockItems();
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
  dateItem = addText(data->created().toString(style().string("date-format")),
		     QFont(style().string("date-font-family"),
			   style().real("date-font-size")));
  dateItem->setDefaultTextColor(style().color("date-color"));
  QPointF br = dateItem->boundingRect().bottomRight();
  dateItem->setPos(style().real("page-width") -
		   style().real("margin-right-over") -
		   br.x(),
		   style().real("margin-top") -
		   style().real("title-sep") -
		   br.y());
}

void PageScene::makeTitleItem() {
  titleItemX = new TitleItem(data->title(), 0);
  titleItem = titleItemX;
  connect(titleItemX,
	  SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	  SLOT(futileTitleMovement(int, Qt::KeyboardModifiers)));
  addItem(titleItem);

  titleItemX->makeWritable();

  nOfNItem = addText("n/N",
		     QFont(style().string("title-font-family"),
			   style().real("title-font-size")));
  nOfNItem->setDefaultTextColor(style().color("pgno-color"));
  
  positionTitleItem();
  
  connect(titleItemX->document(), SIGNAL(contentsChanged()),
	  SLOT(titleEdited()));
}

void PageScene::makeBlockItems() {
  foreach (BlockData *bd, data->blocks()) {
    BlockItem *bi = tryMakeTextBlock(bd);
    if (!bi)
      bi = tryMakeGfxBlock(bd);
    Q_ASSERT(bi);
    connect(bi, SIGNAL(vboxChanged()), vChangeMapper, SLOT(map()));
    blockItems.append(bi);
    sheetNos.append(bd->sheet());
    topY.append(bd->y0());
    FootnoteGroupItem *fng = new FootnoteGroupItem(bd, this);
    foreach (FootnoteItem *fni, fng->itemChildren<FootnoteItem>())
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
  BaseScene::positionTitleItem();

  /* Reposition "n/N" */
  QTextDocument *doc = titleItemX->document();
  QTextBlock blk = doc->lastBlock();
  QTextLayout *lay = blk.layout();
  QTextLine l = lay->lineAt(blk.lineCount()-1);
  QPointF tl(l.cursorToX(blk.length()) + 10, l.y());
  tl = titleItem->mapToScene(tl + lay->position());
  lay = nOfNItem->document()->lastBlock().layout();
  l = lay->lineAt(0);
  tl -= lay->position() - QPointF(l.cursorToX(0), l.y());
  nOfNItem->setPos(tl);
}

void PageScene::stackBlocks() {
  sheetNos.clear();
  topY.clear();
  double y0 = style().real("margin-top");
  for (int n=0; n<blockItems.size(); n++) {
    sheetNos.append(0);
    topY.append(y0);
  }
  restackBlocks(0, true);
}

void PageScene::restackBlocks(int starti, bool preferData) {
  int endi = sheetNos.size();
  if (starti>=endi)
    return;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = topY[starti];
  int sheet = sheetNos[starti];
  double y1a = y1;

  for (int i=0; i<starti; i++)
    if (sheetNos[i]==sheet)
      y1a -= footnoteGroups[i]->netSceneRect().height();
  
  for (int i=starti; i<endi; i++) {
    BlockItem *bi = blockItems[i];
    BlockData *bd = bi->data();
    double h = bi->netSceneRect().height();
    FootnoteGroupItem *fng = footnoteGroups[i];
    double fnh = fng->netSceneRect().height();
    bool updateData = false;
    if (preferData && bd->sheet()>=0) {
      // (sheet,y) information stored in data, we'll use it
      if (bd->sheet()!=sheet) {
	restackFootnotes(sheet);
	sheet = bd->sheet();
      }
      y = bd->y0();
    } else {
      if (y>y0 && y+h>y1a - fnh) {
	restackFootnotes(sheet);
	y = y0;
	y1a = y1;
	sheet++;
      }
      updateData = true;
    }
    topY[i] = y;
    sheetNos[i] = sheet;
    bi->moveBy(0, y - bi->netSceneRect().top());

    if (updateData) {
      BlockData *bd = bi->data();
      bd->setY0(y);
      bd->setSheet(sheet);
    }

    y = y + h;
  }
  restackFootnotes(sheet);
  nSheets = sheet + 1;
}

void PageScene::restackFootnotes(int sheet) {
  double accumh = 0;
  for (int k=0; k<footnoteGroups.size(); k++) 
    if (sheetNos[k] == sheet)
      accumh += footnoteGroups[k]->netSceneRect().height();

  double y = style().real("page-height")
    - style().real("margin-bottom")
    - accumh;
  for (int k=0; k<footnoteGroups.size(); k++) {
    if (sheetNos[k] == sheet) {
      footnoteGroups[k]->setPos(style().real("margin-left"), y);
      y += footnoteGroups[k]->netSceneRect().height();
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

  if (nSheets>1) 
    nOfNItem->setPlainText(QString("(%1/%2)").arg(iSheet+1).arg(nSheets));
  else
    nOfNItem->setPlainText("");

  // Shape below item
  int iLast = findLastBlockOnSheet(iSheet);
  double ytop = iLast<0
    ? style().real("margin-top")
    :  blockItems[iLast]->netSceneRect().bottom();
  belowItem->setRect(style().real("margin-left"),
		     ytop,
		     style().real("page-width")
		     - style().real("margin-left")
		     - style().real("margin-right"),
		     style().real("page-height")
		     - ytop
		     - style().real("margin-bottom"));

  if (oldSheet!=iSheet)
    emit nowOnPage(startPage()+iSheet);
}

int PageScene::findLastBlockOnSheet(int sheet) {
  double maxY = 0;
  int iAbove = -1;
  for (int i=0; i<blockItems.size(); i++) {
    if (sheetNos[i] == sheet) {
      double by = blockItems[i]->netSceneRect().bottom();
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
  
  bi->deleteLater();
  data->deleteBlock(bd);
  fng->deleteLater();

  restackBlocks();

  gotoSheet(iSheet>=nSheets ? nSheets-1 : iSheet);
}

GfxBlockItem *PageScene::newGfxBlock() {
  int iAbove = findLastBlockOnSheet(iSheet);
  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();
  double yt = (iAbove>=0)
    ? blockItems[iAbove]->netSceneRect().bottom()
    : style().real("margin-top");

  GfxBlockData *gbd = new GfxBlockData();
  data->addBlock(gbd);
  GfxBlockItem *gbi = new GfxBlockItem(gbd);
  addItem(gbi);
  gbi->makeWritable();
  FootnoteGroupItem *fng =  new FootnoteGroupItem(gbd, this);
  
  blockItems.insert(iNew, gbi);
  sheetNos.insert(iNew, iSheet);
  topY.insert(iNew, yt);
  footnoteGroups.insert(iNew, fng);

  connect(gbi, SIGNAL(vboxChanged()), vChangeMapper, SLOT(map()));
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
  Q_ASSERT(orig);
  TextBlockData *copy = Data::deepCopy(orig);
  injectTextBlock(copy, iblock);
  topY[iblock] = topY[iblock+1];
  TextBlockItem *tbi_pre = dynamic_cast<TextBlockItem*>(blockItems[iblock]);
  Q_ASSERT(tbi_pre);
  QTextCursor pre = tbi_pre->text()->textCursor();
  pre.setPosition(pos);
  pre.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  pre.deleteChar();
  TextBlockItem *tbi_post = dynamic_cast<TextBlockItem*>(blockItems[iblock+1]);
  Q_ASSERT(tbi_post);
  QTextCursor post = tbi_post->text()->textCursor();
  post.setPosition(pos);
  post.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
  post.deleteChar();
  tbi_post->text()->setTextCursor(post);
  restackBlocks(iblock>0 ? iblock-1 : iblock);
  gotoSheet(sheetNos[iblock+1]);
  tbi_post->setFocus();
}

void PageScene::joinTextBlocks(int iblock_pre, int iblock_post) {
  Q_ASSERT(iblock_pre<iblock_post);
  Q_ASSERT(iblock_pre>=0);
  Q_ASSERT(iblock_post<blockItems.size());
  TextBlockItem *tbi_pre
    = dynamic_cast<TextBlockItem*>(blockItems[iblock_pre]);
  TextBlockItem *tbi_post
    = dynamic_cast<TextBlockItem*>(blockItems[iblock_post]);
  Q_ASSERT(tbi_pre);
  Q_ASSERT(tbi_post);
  QTextCursor c_pre = tbi_pre->text()->textCursor();
  c_pre.movePosition(QTextCursor::End);
  TextData *td_post = tbi_post->data()->text();
  int len = c_pre.position();
  c_pre.insertText(td_post->text());
  TextItem *ti_pre = tbi_pre->text();
  foreach (MarkupData *md, td_post->markups()) {
    MarkupData *copy = Data::deepCopy(md);
    td_post->takeChild(copy); // unparent it
    copy->update(0, 0, len);
    ti_pre->addMarkup(copy);
  }
  c_pre.setPosition(len);
  tbi_pre->text()->setTextCursor(c_pre);
  TextData *td_pre = tbi_pre->data()->text();
  if (td_post->created() < td_pre->created())
    td_pre->setCreated(td_post->created());

  foreach (FootnoteData *fnd, tbi_post->data()->children<FootnoteData>()) {
    FootnoteData *copy = Data::deepCopy(fnd);
    tbi_pre->data()->addChild(copy);
    FootnoteItem *fni = new FootnoteItem(copy, footnoteGroups[iblock_pre]);
    connect(fni, SIGNAL(futileMovement()), SLOT(futileNoteMovement()));
    if (writable)
      fni->makeWritable();
  }
  deleteBlock(iblock_post);
  footnoteGroups[iblock_pre]->restack();
  gotoSheet(sheetNos[iblock_pre]);
  tbi_pre->setFocus();
}  

TextBlockItem *PageScene::injectTextBlock(TextBlockData *tbd, int iblock) {
  // creates a new text block immediately before iblock (or at end if iblock
  // points past the last text block)
  BlockData *tbd_next =  iblock<blockItems.size()
    ? data->blocks()[iblock]
    : 0;
  data->insertBlockBefore(tbd, tbd_next);
  TextBlockItem *tbi = new TextBlockItem(tbd);
  addItem(tbi);
  tbi->makeWritable();

  blockItems.insert(iblock, tbi);
  sheetNos.insert(iblock, iSheet);
  topY.insert(iblock, 0); // this topY is just a place holder
  footnoteGroups.insert(iblock, new FootnoteGroupItem(tbd, this));
  connect(tbi, SIGNAL(vboxChanged()), vChangeMapper, SLOT(map()));
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  remap();
  return tbi;
}

void PageScene::remap() {
  for (int i=0; i<blockItems.size(); i++) {
    BlockItem *bi = blockItems[i];
    hChangeMapper->setMapping(bi, i);
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
    ? blockItems[iAbove]->netSceneRect().bottom()
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
    // no target, go to end of previous
    QTextCursor c = tbi->text()->textCursor();
    c.movePosition(QTextCursor::End);
    tbi->text()->setTextCursor(c);
    return;
  }

  if (fmi.key()==Qt::Key_Delete) {
    joinTextBlocks(block, tgtidx);
    return;
  } else if (fmi.key()==Qt::Key_Backspace) {
    joinTextBlocks(tgtidx, block);
    return;
  }
  
  TextBlockItem *tgt = dynamic_cast<TextBlockItem*>(blockItems[tgtidx]);
  Q_ASSERT(tgt);
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

void PageScene::hChanged(int block) {
  // Never allow sticking out L. of page
  if (blockItems[block]->netSceneRect().left()<0)
     blockItems[block]->moveBy(-blockItems[block]->netSceneRect().left(),
			       0);
      
  // Try to respect L & R margins
  double l_space = blockItems[block]->netSceneRect().left()
    - style().real("margin-left");
  double r_space = style().real("page-width")
    - style().real("margin-right")
    - blockItems[block]->netSceneRect().right();
  if (l_space<0) {
    double dx = -l_space;
    if (dx>r_space)
      dx = r_space;
    if (dx>0)
      blockItems[block]->moveBy(dx, 0);
  } else if (r_space<0) {
    double dx = -r_space;
    if (dx>l_space)
      dx = l_space;
    if (dx>0)
      blockItems[block]->moveBy(-dx, 0);
  }
}

void PageScene::vChanged(int block) {
  restackBlocks(block);
  gotoSheet(sheetNos[block]);
}

void PageScene::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  // qDebug() << "PageScene::mousePressEvent";
  QPointF sp = e->scenePos();
  if (inMargin(sp)) {
    //qDebug() << "  in margin";
    QGraphicsScene::mousePressEvent(e);
  } else if (belowContent(sp)) {
    //qDebug() << "  below content";
    if (writable)
      newTextBlock();
    else
      qDebug() << "PageScene::mousePressEvent: Not writable";
  } else {
    QGraphicsScene::mousePressEvent(e);
  }
}

void PageScene::keyPressEvent(QKeyEvent *e) {
  if (e->modifiers() & Qt::ControlModifier) {
    bool steal = false;
    switch (e->key()) {
    case Qt::Key_V:
      steal = tryToPaste();
      break;
    case Qt::Key_N:
      steal = tryMakeNote();
      break;
    }
    if (steal) {
      e->accept();
      return;
    }
  }
  BaseScene::keyPressEvent(e);
}

bool PageScene::tryMakeNote() {
  //  qDebug() << "PageScene::tryMakeNote";
  TextItem *ti = dynamic_cast<TextItem*>(focusItem());
  if (!ti) {
    // qDebug() << "Focus not in text item";
    return false;
  }
  
  TextBlockItem *tbi = dynamic_cast<TextBlockItem*>(ti->parentItem());
  if (!tbi) {
    // qDebug() << "Focus not in text block item";
    return false;
  }
  for (int i=0; i<blockItems.size(); i++) {
    if (blockItems[i] == tbi) {
      // qDebug() << "  Focus in block " << i;
      QTextCursor c = ti->textCursor();
      int pos = c.position();
      TextData *t = tbi->data()->text();
      foreach (MarkupData *md, t->children<MarkupData>()) {
	if (md->style()==MarkupData::CustomRef
	    && md->end()>=pos
	    && md->start()<=pos) {
	  // qDebug() << "  Found a custom ref";
	  QTextCursor c = ti->textCursor();
	  c.setPosition(md->start());
	  c.setPosition(md->end(), QTextCursor::KeepAnchor);
	  QString tag = c.selectedText();
	  newFootnote(i, tag);
	  return true;
	}
      }
      //      qDebug() << "  No customref found";
      return false;
    }
  }
  //  qDebug() << "  Unknown block!!?";
  return false;
}
	

bool PageScene::tryToPaste() {
  // we get it first.
  // if we don't send the event on to QGraphicsScene, textItems don't get it
  qDebug() << "PageScene::tryToPaste";
  return false;
}

void PageScene::dropEvent(QGraphicsSceneDragDropEvent *e) {
  //qDebug() << " scenePos = " << e->scenePos();
  //qDebug() << " action = " << e->dropAction();
  //qDebug() << " modifiers = " << e->modifiers();
  //qDebug() << " source = " << e->source() << " (views="<<views()<<")";

  if (e->source() == 0) {
    // event from outside our application
    QMimeData const *md = e->mimeData();
    bool accept = false;
    if (md->hasImage()) 
      accept = importDroppedImage(e->scenePos(),
				   qvariant_cast<QImage>(e->mimeData()
							 ->imageData()));
    else if (md->hasUrls()) 
      accept = importDroppedUrls(e->scenePos(), md->urls());
    else if (md->hasText())
      accept = importDroppedText(e->scenePos(), md->text());

    if (accept) {
      e->setDropAction(Qt::CopyAction);
      e->accept();
    } else {
      e->ignore(); // this does not seem to graphically refuse the offering
    }
  } else {
    // event from inside our application
    // we may be dragging some image around
    // not yet implemented
    qDebug() << "PageScene: internal drop";
    QGraphicsScene::dropEvent(e);
  }
}

bool PageScene::importDroppedImage(QPointF scenePos, QImage const &img,
				   QUrl const *source) {
  // Return true if we want it
  GfxBlockItem *dst = dynamic_cast<GfxBlockItem *>(itemAt(scenePos));
  if (!dst)
    dst = newGfxBlock();
  dst->newImage(img, source, dst->mapFromScene(scenePos));
  return true;
}

bool PageScene::importDroppedUrls(QPointF scenePos, QList<QUrl> const &urls) {
  bool ok = false;
  foreach (QUrl const &u, urls)
    if (importDroppedUrl(scenePos, u))
      ok = true;
  return ok;
}

bool PageScene::importDroppedUrl(QPointF scenePos, QUrl const &url) {
  // QGraphicsItem *dst = itemAt(scenePos);
  /* A URL could be any of the following:
     (1) A local image file
     (2) A local file of non-image type
     (3) An internet-located image file
     (4) A web-page
     (5) Anything else
  */
  if (url.isLocalFile()) {
    QImage image = QImage(url.toLocalFile());
    if (!image.isNull())
      return importDroppedImage(scenePos, image, &url);
    else
      return importDroppedFile(scenePos, url.toLocalFile());
  } else if (networkManager) {
    networkManager->get(QNetworkRequest(url));
    return true;
  } else {
    // no network
    return importDroppedText(scenePos, url.toString());
  }
  return false;
}

bool PageScene::importDroppedText(QPointF scenePos, QString const &txt,
					QUrl const *source) {
  qDebug() << "PageScene: import dropped text: " << scenePos << txt << source;
  return false;
}

bool PageScene::importDroppedFile(QPointF scenePos, QString const &fn) {
  qDebug() << "PageScene: import dropped file: " << scenePos << fn;
  return false;
}
    
void PageScene::makeWritable() {
  writable = true;
  belowItem->setCursor(Qt::IBeamCursor);
  belowItem->setAcceptDrops(true);
  foreach (BlockItem *bi, blockItems)
    bi->makeWritable();
  foreach (FootnoteGroupItem *fng, footnoteGroups)
    fng->makeWritable();
}

int PageScene::startPage() const {
  return data->startPage();
}

bool PageScene::isWritable() const {
  return writable;
}

void PageScene::newFootnote(int block, QString tag) {
  Q_ASSERT(block>=0 && block<blockItems.size());
  
  FootnoteData *fnd = new FootnoteData(blockItems[block]->data());
  fnd->setTag(tag);
  FootnoteItem *fni = new FootnoteItem(fnd, footnoteGroups[block]);
  connect(fni, SIGNAL(futileMovement()), SLOT(futileNoteMovement()));
  fni->makeWritable();
  fni->setFocus();
  footnoteGroups[block]->restack();
}

void PageScene::noteVChanged(int block) {
  Q_ASSERT(block>=0 && block<blockItems.size());
  int s = sheetNos[block];
  for (int i=0; i<block; i++) {
    if (sheetNos[i]==s) {
      restackBlocks(i);
      return;
    }
  }
  restackBlocks(block);
}

void PageScene::futileNoteMovement() {
}
