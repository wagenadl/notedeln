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
  QGraphicsScene(parent),
  data(data),
  style(Style::defaultStyle()) {

  networkManager = 0;
  
  hChangeMapper = new QSignalMapper(this);
  vChangeMapper = new QSignalMapper(this);
  futileMovementMapper = new QSignalMapper(this);
  enterPressedMapper = new QSignalMapper(this);
  connect(hChangeMapper, SIGNAL(mapped(int)), SLOT(hChanged(int)));
  connect(vChangeMapper, SIGNAL(mapped(int)), SLOT(vChanged(int)));
  connect(futileMovementMapper, SIGNAL(mapped(int)), SLOT(futileMovement(int)));
  connect(enterPressedMapper, SIGNAL(mapped(int)), SLOT(enterPressed(int)));
  
  mode_ = TypeMode; // really, this should be ViewMode, I think
  makeBackground();
  makeDateItem();
  makePgNoItem();
  makeContdItem();
  makeTitleItem();
  makeBlockItems();
  stackBlocks();
  gotoSheet(0);
}

void PageScene::makeBackground() {
  setSceneRect(0,
	       0,
	       style["page-width"].toDouble(),
	       style["page-height"].toDouble());

  setBackgroundBrush(QBrush(QColor(style["border-color"].toString())));
  bgItem = addRect(0,
		   0, 
		   style["page-width"].toDouble(),
		   style["page-height"].toDouble(),
		   QPen(Qt::NoPen),
		   QBrush(QColor(style["background-color"].toString())));
  bgItem->setAcceptDrops(true);
			
  leftMarginItem = addLine(style["margin-left"].toDouble(),
			   0,
			   style["margin-left"].toDouble(),
			   height(),
			   QPen(QBrush(QColor(style["margin-left-line-color"].
					      toString())),
				style["margin-left-line-width"].toDouble())
			   );
  
  topMarginItem = addLine(0,
			  style["margin-top"].toDouble(),
			  width(),
			  style["margin-top"].toDouble(),
			  QPen(QBrush(QColor(style["margin-top-line-color"].
					     toString())),
			       style["margin-top-line-width"].toDouble())
			  );

}

void PageScene::makeDateItem() {
  Q_ASSERT(data);
  qDebug() << "data: " << data << " data-created: " << data->created();
  dateItem = addText(data->created().toString("MM/dd/yyyy"),
		     QFont(style["date-font-family"].toString(),
			   style["date-font-size"].toDouble())
		     );
  dateItem->setDefaultTextColor(QColor(style["date-color"].toString()));
  QPointF br = dateItem->boundingRect().bottomRight();
  dateItem->setPos(style["page-width"].toDouble() -
		   style["margin-right-over"].toDouble() -
		   br.x(),
		   style["margin-top"].toDouble() -
		   style["title-sep"].toDouble() -
		   br.y()
		   );
}

void PageScene::makePgNoItem() {
  pgNoItem = addText(QString::number(data->startPage() + iSheet),
		     QFont(style["pgno-font-family"].toString(),
			   style["pgno-font-size"].toDouble())
		     );
  pgNoItem->setDefaultTextColor(QColor(style["pgno-color"].toString()));
  positionPgNoItem();
}

void PageScene::makeContdItem() {
  contdItem = addText(">",
		     QFont(style["contd-font-family"].toString(),
			   style["contd-font-size"].toDouble())
		     );
  contdItem->setDefaultTextColor(QColor(style["contd-color"].toString()));
  QPointF tr = contdItem->boundingRect().topRight();
  contdItem->setPos(style["margin-left"].toDouble()
		    - tr.x(),
		    style["margin-top"].toDouble()
		    - tr.y()
		    );

  contItem = addText(">",
		     QFont(style["contd-font-family"].toString(),
			   style["contd-font-size"].toDouble())
		     );
  contItem->setDefaultTextColor(QColor(style["contd-color"].toString()));
  QPointF bl = contItem->boundingRect().bottomLeft();
  contItem->setPos(style["page-width"].toDouble()
		   - style["margin-right"].toDouble()
		   - bl.x(),
		   style["page-height"].toDouble()
		   + style["margin-bottom"].toDouble()
		   - bl.y()
		    );
}

void PageScene::positionPgNoItem() {
  QPointF tr = pgNoItem->boundingRect().topRight();
  pgNoItem->setPos(style["page-width"].toDouble() -
		   style["margin-right-over"].toDouble() -
		   tr.x(),
		   style["page-height"].toDouble() -
		   style["margin-bottom"].toDouble() +
		   style["pgno-sep"].toDouble() -
		   tr.y()
		   );
}

void PageScene::makeTitleItem() {
  Q_ASSERT(dateItem);
  Q_ASSERT(data);

  titleItem = new TitleItem(data->title(), 0);
  addItem(titleItem);

  nOfNItem = addText("n/N",
		     QFont(style["title-font-family"].toString(),
			   style["title-font-size"].toDouble())
		     );
  nOfNItem->setDefaultTextColor(QColor(style["pgno-color"].toString()));
  
  positionTitleItem();
  
  connect(titleItem->text()->document(), SIGNAL(contentsChanged()),
	  SLOT(titleEdited()));
}

void PageScene::makeBlockItems() {
  foreach (BlockData *bd, data->blocks()) {
    qDebug() << "PageScene: considering block data";
    TextBlockData *tbd = dynamic_cast<TextBlockData*>(bd);
    if (tbd) {
      qDebug() << "It's a text block";
      TextBlockItem *tbi = new TextBlockItem(tbd, this);
      int iNew = blockItems.size();
      blockItems.append(tbi);
      sheetNos.append(tbd->sheet());
      topY.append(tbd->y0());
      vChangeMapper->setMapping(tbi, iNew);
      futileMovementMapper->setMapping(tbi, iNew);
      enterPressedMapper->setMapping(tbi, iNew);
      connect(tbi, SIGNAL(vboxChanged()), vChangeMapper, SLOT(map()));
      connect(tbi, SIGNAL(futileMovement()), futileMovementMapper, SLOT(map()));
      connect(tbi, SIGNAL(enterPressed()), enterPressedMapper, SLOT(map()));
    }
  }
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
  double dateX = dateItem->mapToScene(dateItem->boundingRect().topLeft()).x();
  titleItem->text()->setTextWidth(dateX - style["margin-left"].toDouble() - 5);
  QPointF bl = titleItem->boundingRect().bottomLeft();
  titleItem->setPos(style["margin-left"].toDouble() -
		    bl.x(),
		    style["margin-top"].toDouble() -
		    style["title-sep"].toDouble() -
		    bl.y()
		    );

  /* Reposition "n/N" */
  QTextDocument *doc = titleItem->text()->document();
  QTextBlock blk = doc->lastBlock();
  QTextLayout *lay = blk.layout();
  QTextLine l = lay->lineAt(blk.lineCount()-1);
  QPointF tl(l.cursorToX(blk.length()) + 10, l.y());
  tl = titleItem->text()->mapToScene(tl + lay->position());
  lay = nOfNItem->document()->lastBlock().layout();
  l = lay->lineAt(0);
  tl -= lay->position() - QPointF(l.cursorToX(0), l.y());
  nOfNItem->setPos(tl);
}

void PageScene::stackBlocks() {
  Q_ASSERT(contdItem!=0);
  Q_ASSERT(contItem!=0);
  
  sheetNos.clear();
  int sheet = 0;
  double y0 = style["margin-top"].toDouble();
  double y1 = style["page-height"].toDouble()
    - style["margin-bottom"].toDouble();
  double y = y0;

  foreach (BlockItem *bi, blockItems) {
    BlockData *bd = bi->data();
    double h = bi->sceneBoundingRect().height();
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

    bi->moveBy(0, y-bi->sceneBoundingRect().top());

    y += h;
  }

  nSheets = sheet+1;
}

int PageScene::restackBlocks(int starti) {
  int endi = sheetNos.size();
  if (starti>=endi)
    return 0;
  double y0 = style["margin-top"].toDouble();
  double y1 = style["page-height"].toDouble()
    - style["margin-bottom"].toDouble();
  double y = topY[starti];
  int sheet = sheetNos[starti];
  for (int i=starti; i<endi; i++) {
    BlockItem *bi = blockItems[i];
    double h = bi->sceneBoundingRect().height();
    if (y>y0 && y+h>y1) {
      y = y0;
      sheet++;
    }

    bi->moveBy(0, y - bi->sceneBoundingRect().top());
    topY[i] = y;
    sheetNos[i] = sheet;
    BlockData *bd = bi->data();
    bd->setY0(y);
    bd->setSheet(sheet);
    qDebug() << "Restack i="<<i<< " y="<<y << " h="<<h << " sheet="<<sheet;
    
    y = y + h;
  }
  return sheetNos[starti];
}

void PageScene::previousSheet() {
  if (iSheet>0)
    gotoSheet(iSheet-1);
}

void PageScene::nextSheet() {
  if (iSheet<nSheets-1)
    gotoSheet(iSheet+1);
}

void PageScene::gotoSheet(int i) {
  Q_ASSERT(data);
  Q_ASSERT(titleItem);
  Q_ASSERT(nOfNItem);
  
  iSheet = i;
  if (iSheet>=nSheets)
    iSheet = nSheets-1;
  if (iSheet<0)
    iSheet = 0;

  // Set visibility for all blocks
  int nBlocks = blockItems.size();
  for (int k=0; k<nBlocks; k++)
    blockItems[k]->setVisible(sheetNos[k]==iSheet);

  if (nSheets>1) 
    nOfNItem->setPlainText(QString("(%1/%2)").arg(iSheet+1).arg(nSheets));
  else
    nOfNItem->setPlainText("");

  // Set visibility of continuation markers
  contdItem->setVisible(iSheet>0);
  contItem->setVisible(iSheet+1 < nSheets);

  // Set page number
  pgNoItem->setPlainText(QString::number(data->startPage() + iSheet));
  positionPgNoItem();
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

bool PageScene::inMargin(QPointF sp) {

  return sp.x() < style["margin-left"].toDouble()
    || sp.x() >= style["page-width"].toDouble()
       - style["margin-right"].toDouble()
    || sp.y() < style["margin-top"].toDouble()
    || sp.y() >= style["page-height"].toDouble()
       - style["margin-bottom"].toDouble();
}    

bool PageScene::belowContent(QPointF sp) {
  int iAbove = findLastBlockOnSheet(iSheet);
  if (iAbove<0)
    return true;

  return sp.y() >= blockItems[iAbove]->netSceneRect().bottom();
}

void PageScene::deleteBlock(int blocki) {
  Q_ASSERT(data);
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

void PageScene::newTextBlock(int iAbove, bool evenIfLastEmpty) {
  qDebug() << "newTextBlock " << iAbove;
  Q_ASSERT(data);

  if (iAbove<0)
    iAbove = findLastBlockOnSheet(iSheet);

  if (iAbove>=0 && !evenIfLastEmpty) {
    TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[iAbove]);
    if (tbi && tbi->document()->isEmpty()) {
      // Previous block is empty text, go there instead
      tbi->setFocus();
      return;
    }
  }      

  int iNew = (iAbove>=0)
    ? iAbove + 1
    : blockItems.size();
  double yt = (iAbove>=0)
    ? blockItems[iAbove]->sceneBoundingRect().bottom()
    : style["margin-top"].toDouble();

  TextBlockData *tbd = new TextBlockData();
  data->addBlock(tbd);
  TextBlockItem *tbi = new TextBlockItem(tbd, this);
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
}

void PageScene::futileMovement(int block) {
  qDebug() << "futilemovement " << block;
  TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[block]);
  if (!tbi) {
    qDebug() << "not a text block";
    return;
  }

  TextBlockItem::FutileMovementInfo fmi = tbi->lastFutileMovement();
  qDebug() << "got info";
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
    qDebug() << "No target";
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
  qDebug() << "new paragraph " << block;
  TextBlockItem *tbi = dynamic_cast<TextBlockItem *>(blockItems[block]);
  if (!tbi
      || !tbi->text()->document()->isEmpty()
      || style["paragraph-allow-empty"].toInt()!=0)
    newTextBlock(block, true);
}

void PageScene::hChanged(int block) {
  // Never allow sticking out L. of page
  if (blockItems[block]->sceneBoundingRect().left()<0)
     blockItems[block]->moveBy(-blockItems[block]->sceneBoundingRect().left(),
			       0);
      
  // Try to respect L & R margins
  double l_space = blockItems[block]->sceneBoundingRect().left()
    - style["margin-left"].toDouble();
  double r_space = style["page-width"].toDouble()
    - style["margin-right"].toDouble()
    - blockItems[block]->sceneBoundingRect().right();
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
  QPointF sp = e->scenePos();
  if (inMargin(sp)) {
    QGraphicsScene::mousePressEvent(e);
  } else if (belowContent(sp) && mode_==TypeMode) {
    newTextBlock();
  } else {
    QGraphicsScene::mousePressEvent(e);
  }
}
	
void PageScene::setMode(PageScene::MouseMode m) {
  mode_ = m;
}

PageScene::MouseMode PageScene::mode() const {
  return mode_;
}

void PageScene::keyPressEvent(QKeyEvent *e) {
  if (e->key()==Qt::Key_V && e->modifiers() & Qt::ControlModifier) 
    tryToPaste();
  QGraphicsScene::keyPressEvent(e);
}

void PageScene::tryToPaste() {
  // we get it first.
  // if we don't send the event on to QGraphicsScene, textItems don't get it
  qDebug() << "PageScene::tryToPaste";
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
  qDebug() << "PageScene: import dropped image";
  QString imgName = ImageCacheManager::import(img, source);
  /* ImageCacheManager must somehow be local to a notebook, but it is
     not obvious right now how to implement that.
     Perhaps I am simply putting too much weight on the PageScene.
  */
  GfxBlockItem *dst = dynamic_cast<GfxBlockItem *>(itemAt(scenePos));
  if (!dst)
    dst = newGfxBlock();
  dst->addImage(imgName);
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
  QGraphicsItem *dst = itemAt(scenePos);
  qDebug() << "PageScene: import dropped url: " << url
	   << " at " << scenePos
	   << " onto " << dst;
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
  qDebug() << "PageScene: import dropped text: " << txt;
  return false;
}

bool PageScene::importDroppedFile(QPointF scenePos, QString const &fn) {
  qDebug() << "PageScene: import dropped file: " << fn;
  return false;
}
    
