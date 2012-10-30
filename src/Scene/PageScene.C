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
  
  hChangeMapper = new QSignalMapper(this);
  vChangeMapper = new QSignalMapper(this);
  futileMovementMapper = new QSignalMapper(this);
  enterPressedMapper = new QSignalMapper(this);
  connect(hChangeMapper, SIGNAL(mapped(int)), SLOT(hChanged(int)));
  connect(vChangeMapper, SIGNAL(mapped(int)), SLOT(vChanged(int)));
  connect(futileMovementMapper, SIGNAL(mapped(int)), SLOT(futileMovement(int)));
  connect(enterPressedMapper, SIGNAL(mapped(int)), SLOT(enterPressed(int)));

  nOfNItem = 0;
  dateItem = 0;
}

void PageScene::populate() {
  BaseScene::populate();
  makeDateItem();
  positionTitleItem();
  makeBlockItems();
  stackBlocks();
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
  TitleItem *tt = new TitleItem(data->title(), 0);
  titleItem = tt;
  addItem(titleItem);

  tt->makeWritable();

  nOfNItem = addText("n/N",
		     QFont(style().string("title-font-family"),
			   style().real("title-font-size")));
  nOfNItem->setDefaultTextColor(style().color("pgno-color"));
  
  positionTitleItem();
  
  connect(tt->document(), SIGNAL(contentsChanged()),
	  SLOT(titleEdited()));
}

void PageScene::makeBlockItems() {
  foreach (BlockData *bd, data->blocks()) {
    qDebug() << "PageScene: considering block data";
    BlockItem *bi = tryMakeTextBlock(bd);
    if (!bi)
      bi = tryMakeGfxBlock(bd);
    Q_ASSERT(bi);
    bi->makeWritable(); // more sophistication needed
    vChangeMapper->setMapping(bi, blockItems.size());
    connect(bi, SIGNAL(vboxChanged()), vChangeMapper, SLOT(map()));
    blockItems.append(bi);
    sheetNos.append(bd->sheet());
    topY.append(bd->y0());
  }
}

BlockItem *PageScene::tryMakeGfxBlock(BlockData *bd) {
  GfxBlockData *gbd = dynamic_cast<GfxBlockData*>(bd);
  if (!gbd)
    return 0;
  GfxBlockItem *gbi = new GfxBlockItem(gbd, this);
  return gbi;
}

BlockItem *PageScene::tryMakeTextBlock(BlockData *bd) {
  TextBlockData *tbd = dynamic_cast<TextBlockData*>(bd);
  if (!tbd)
    return 0;

  TextBlockItem *tbi = new TextBlockItem(tbd, this);
  
  futileMovementMapper->setMapping(tbi, blockItems.size());
  enterPressedMapper->setMapping(tbi, blockItems.size());
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  connect(tbi, SIGNAL(enterPressed()), enterPressedMapper, SLOT(map()));
  return tbi;
}
  
PageScene::~PageScene() {
}

// void PageScene::titleTextEdited() {
//   /* This crazy piece of code replaces new lines in title by spaces */
//   QTextBlock blk = titleItem->text()->document()->lastBlock();
//   while (blk.position()>0) {
//     QTextCursor c(blk);
//     c.deletePreviousChar();
//     c.insertText(" ");
//     blk = titleItem->text()->document()->lastBlock();
//   }
// }

void PageScene::titleEdited() {
  positionTitleItem();
}

void PageScene::positionTitleItem() {
  /* This keeps the title bottom aligned */
  if (!dateItem || !nOfNItem)
    return; // too early in process

  double dateX = dateItem->mapToScene(dateItem->boundingRect().topLeft()).x();
  titleItem->setTextWidth(dateX - style().real("margin-left")
			  - style().real("title-sep") - 5);
  BaseScene::positionTitleItem();

  /* Reposition "n/N" */
  QTextDocument *doc = titleItem->document();
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
  int sheet = 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = y0;

  foreach (BlockItem *bi, blockItems) {
    BlockData *bd = bi->data();
    double h = bi->netSceneRect().height();
    if (bd->sheet()>=0) {
      // (sheet,y) information stored in data, we'll use it
      sheet = bd->sheet();
      y = bd->y0();
    } else {
      // no (sheet,y) information stored in data, we'll make our own
      if (y!=y0 && y+h>y1) {
	sheet ++;
	y = y0;
      }
      bd->setSheet(sheet);
      bd->setY0(y);
    }
    sheetNos.append(sheet);
    topY.append(y);
    bi->moveBy(0, y-bi->netSceneRect().top());

    y += h;
  }

  nSheets = sheet+1;
}

int PageScene::restackBlocks(int starti) {
  int endi = sheetNos.size();
  if (starti>=endi)
    return 0;
  double y0 = style().real("margin-top");
  double y1 = style().real("page-height") - style().real("margin-bottom");
  double y = topY[starti];
  int sheet = sheetNos[starti];
  for (int i=starti; i<endi; i++) {
    BlockItem *bi = blockItems[i];
    double h = bi->netSceneRect().height();
    if (y>y0 && y+h>y1) {
      y = y0;
      sheet++;
    }

    bi->moveBy(0, y - bi->netSceneRect().top());
    topY[i] = y;
    sheetNos[i] = sheet;
    BlockData *bd = bi->data();
    bd->setY0(y);
    bd->setSheet(sheet);
    
    y = y + h;
  }
  return sheetNos[starti];
}

void PageScene::gotoSheet(int i) {
  BaseScene::gotoSheet(i);

  // Set visibility for all blocks
  int nBlocks = blockItems.size();
  for (int k=0; k<nBlocks; k++)
    blockItems[k]->setVisible(sheetNos[k]==iSheet);

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
  int iAbove = findLastBlockOnSheet(iSheet);
  if (iAbove<0)
    return true;
  else
    return sp.y() >= blockItems[iAbove]->netSceneRect().bottom();
}

void PageScene::deleteBlock(int blocki) {
  if (blocki>=blockItems.size()) {
    qDebug() << "PageScene: deleting nonexisting block " << blocki;
    return;
  }
  BlockItem *bi = blockItems[blocki];
  BlockData *bd = bi->data();

  blockItems.removeAt(blocki);
  sheetNos.removeAt(blocki);
  topY.removeAt(blocki);

  delete bi;
  data->deleteBlock(bd);

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
  GfxBlockItem *gbi = new GfxBlockItem(gbd, this);
  gbi->makeWritable();
  
  blockItems.insert(iNew, gbi);
  sheetNos.insert(iNew, iSheet);
  topY.insert(iNew, yt);

  vChangeMapper->setMapping(gbi, iNew);
  connect(gbi, SIGNAL(vboxChanged()), vChangeMapper, SLOT(map()));

  restackBlocks(iNew);
  gotoSheet(sheetNos[iNew]);
  return gbi;
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
  data->addBlock(tbd);
  TextBlockItem *tbi = new TextBlockItem(tbd, this);
  tbi->makeWritable();
  
  blockItems.insert(iNew, tbi);
  sheetNos.insert(iNew, iSheet);
  topY.insert(iNew, yt);

  vChangeMapper->setMapping(tbi, iNew);
  futileMovementMapper->setMapping(tbi, iNew);
  enterPressedMapper->setMapping(tbi, iNew);
  connect(tbi, SIGNAL(vboxChanged()), vChangeMapper, SLOT(map()));
  connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
  connect(tbi, SIGNAL(enterPressed()), enterPressedMapper, SLOT(map()));

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

  TextBlockItem::FutileMovementInfo fmi = tbi->lastFutileMovement();
  TextBlockItem *tgt = 0;
  if (fmi.key()==Qt::Key_Left || fmi.key()==Qt::Key_Up) {
    for (int b=block-1; b>=0; b--) {
      tgt = dynamic_cast<TextBlockItem *>(blockItems[b]);
      if (tgt)
	break;
    }
  } else if (fmi.key()==Qt::Key_Right || fmi.key()==Qt::Key_Down) {
    for (int b=block+1; b<blockItems.size(); b++) {
      tgt = dynamic_cast<TextBlockItem *>(blockItems[b]);
      if (tgt) {
	// I should also check that this item is editable
	break;
      }
    }
  }
  if (!tgt) {
    return;
  }

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
    QTextLine l = lay->lineAt(blk.lineCount()-1);
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

void PageScene::enterPressed(int block) {
  TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[block]);
  if (!tbi
      || !tbi->text()->document()->isEmpty()
      || style().flag("paragraph-allow-empty"))
    newTextBlock(block, true);
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
  //qDebug() << "PageScene::mousePressEvent";
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
  switch (e->key()) {
  case Qt::Key_V:
    if (e->modifiers() & Qt::ControlModifier) 
      if (tryToPaste()) {
	e->accept();
	return;
      }
  }
  BaseScene::keyPressEvent(e);
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
  qDebug() << "PageScene: made writable";
  writable = true;
  belowItem->setCursor(Qt::IBeamCursor);
  belowItem->setAcceptDrops(true);
}

int PageScene::startPage() const {
  return data->startPage();
}
