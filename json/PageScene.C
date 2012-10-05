// PageScene.C

#include "PageScene.H"
#include "Style.H"
#include "BlockData.H"
#include "BlockItem.H"
#include "TextBlockItem.H"
#include "TextBlockData.H"
#include "PageData.H"

#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QDateTime>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

PageScene::PageScene(PageData *data, QObject *parent):
  QGraphicsScene(parent),
  data(data),
  style(Style::defaultStyle()) {
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
	       style["margin-left"].toDouble() +
	       style["text-width"].toDouble() +
	       style["margin-right"].toDouble(),
	       style["margin-top"].toDouble() +
	       style["text-height"].toDouble() +
	       style["margin-bottom"].toDouble()
	       );

  setBackgroundBrush(QBrush(QColor(style["border-color"].toString())));
  bgItem = addRect(0,
		   0, 
		   style["margin-left"].toDouble()
		   + style["text-width"].toDouble()
		   + style["margin-right"].toDouble(),
		   style["margin-top"].toDouble()
		   + style["text-height"].toDouble()
		   + style["margin-bottom"].toDouble(),
		   QPen(Qt::NoPen),
		   QBrush(QColor(style["background-color"].toString())));
			
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
  dateItem->setPos(style["margin-left"].toDouble() +
		   style["text-width"].toDouble() +
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
  contItem->setPos(style["margin-left"].toDouble()
		   + style["text-width"].toDouble()
		   - bl.x(),
		   style["margin-top"].toDouble()
		   + style["text-height"].toDouble()
		   - bl.y()
		    );
}

void PageScene::positionPgNoItem() {
  QPointF tr = pgNoItem->boundingRect().topRight();
  pgNoItem->setPos(style["margin-left"].toDouble() +
		   style["text-width"].toDouble() +
		   style["margin-right-over"].toDouble() -
		   tr.x(),
		   style["margin-top"].toDouble() +
		   style["text-height"].toDouble() +
		   style["pgno-sep"].toDouble() -
		   tr.y()
		   );
}

void PageScene::makeTitleItem() {
  Q_ASSERT(dateItem);
  Q_ASSERT(data);
  
  titleItem = addText(data->title(),
		      QFont(style["title-font-family"].toString(),
			    style["title-font-size"].toDouble()));
  titleItem->setDefaultTextColor(QColor(style["title-color"].toString()));
  positionTitleItem();
  titleItem->setTextInteractionFlags(Qt::TextEditorInteraction);
  connect(titleItem->document(), SIGNAL(contentsChanged()),
	  this, SLOT(titleEdited()));
  connect(titleItem->document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(titleTextEdited()));
}

void PageScene::makeBlockItems() {
  // ...
}
  
PageScene::~PageScene() {
}

void PageScene::titleTextEdited() {
  /* This crazy piece of code replaces new lines in title by spaces */
  QTextBlock blk = titleItem->document()->lastBlock();
  while (blk.position()>0) {
    QTextCursor c(blk);
    c.deletePreviousChar();
    c.insertText(" ");
    blk = titleItem->document()->lastBlock();
  }
}

void PageScene::titleEdited() {
  positionTitleItem();
}

void PageScene::positionTitleItem() {
  /* This keeps the title bottom aligned */
  titleItem->setTextWidth(dateItem->mapToScene(dateItem
					       ->boundingRect().topLeft())
			  .x()
			  - style["margin-left"].toDouble()
			  - 5);
  QPointF bl = titleItem->boundingRect().bottomLeft();
  titleItem->setPos(style["margin-left"].toDouble() -
		    bl.x(),
		    style["margin-top"].toDouble() -
		    style["title-sep"].toDouble() -
		    bl.y()
		    );

  // Now we should also update the underlying PageData!
}

void PageScene::stackBlocks() {
  Q_ASSERT(contdItem!=0);
  Q_ASSERT(contItem!=0);
  
  sheetNos.clear();
  int sheet = 0;
  double y0 = style["margin-top"].toDouble();
  double y1 = y0 + style["text-height"].toDouble();
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
  double y1 = y0 + style["text-height"].toDouble();
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
  
  iSheet = i;
  if (iSheet>=nSheets)
    iSheet = nSheets-1;
  if (iSheet<0)
    iSheet = 0;

  // Set visibility for all blocks
  int nBlocks = blockItems.size();
  for (int k=0; k<nBlocks; k++)
    blockItems[k]->setVisible(sheetNos[k]==iSheet);

  // Remove previous "(n/N)" from title
  QTextCursor c = titleItem->document()->find(QRegExp("\\s*\\(\\d+/\\d+)$"));
  if (!c.isNull())
    c.insertText("");

  if (nSheets>1) {
    // add new "(n/N)" to title
    QTextCursor c(titleItem->document());
    c.movePosition(QTextCursor::End);
    c.insertText(QString(" (%1/%2)").arg(iSheet+1).arg(nSheets));
  }

  // Set visibility of continuation markers
  contdItem->setVisible(iSheet>0);
  contItem->setVisible(iSheet+1 < nSheets);

  // Set page number
  pgNoItem->setPlainText(QString::number(data->startPage() + iSheet));
  positionPgNoItem();
}

void PageScene::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  QPointF sp = e->scenePos();
  Style const &style(Style::defaultStyle());
  qDebug() << "Scene mouse press: " << sp;

  double ytop = style["margin-top"].toDouble();
  double xleft = style["margin-left"].toDouble();
  double ybottom = ytop + style["text-height"].toDouble();
  double xright = xleft + style["text-width"].toDouble();

  if (sp.x()<xleft || sp.x()>=xright ||
      sp.y()<ytop || sp.y()>=ybottom) {
    qDebug() << "PageScene: margin press";
    QGraphicsScene::mousePressEvent(e);
    return;
  }
  
  double maxY = ytop;
  int iAbove = -1;
  for (int i=0; i<blockItems.size(); i++) {
    if (sheetNos[i] == iSheet) {
      double by = blockItems[i]->contentsSceneRect().bottom();
      if (by>maxY) {
	maxY = by;
	iAbove = i;
      }
    }
  }
  qDebug() << "maxY: " << maxY;

  if (sp.y() >= maxY) {
    qDebug() << "PageScene: click in empty area at bottom";
    if (mode_==TypeMode) {
      Q_ASSERT(data);
      TextBlockData *tbd = new TextBlockData();
      data->addBlock(tbd);
      int iNext = (iAbove>=0) ? iAbove+1 : blockItems.size();
      TextBlockItem *tbi = new TextBlockItem(tbd, this);
      double yt = iNext<blockItems.size() ? topY[iNext] : ytop;
      blockItems.insert(iNext, tbi);
      sheetNos.insert(iNext, iSheet);
      topY.insert(iNext, yt);
      restackBlocks();
      gotoSheet(sheetNos[iSheet]);
      tbi->setFocus();
    } else {
      QGraphicsScene::mousePressEvent(e);
    }
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
