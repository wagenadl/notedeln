// PageScene.C

#include "PageScene.H"
#include "Style.H"
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QDateTime>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QDebug>

PageScene::PageScene(PageData *data, QObject *parent):
  QGraphicsScene(parent),
  data(data),
  style(Style::defaultStyle()) {
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
  dateItem = addText(QDateTime::currentDateTime().toString("MM/dd/yyyy")
		     /* data->created().toString("MM/dd/yyyy") */,
		     QFont(style["date-font-family"].toString(),
			   style["date-font-size"].toDouble())
		     );
  dateItem->setDefaultTextColor(QColor(style["date-color"].toString()));
  QPointF br = dateItem->boundingRect().bottomRight();
  dateItem->setPos(style["margin-left"].toDouble() +
		   style["text-width"].toDouble() +
		   style["margin-right"].toDouble()*.75 -
		   br.x(),
		   style["margin-top"].toDouble() -
		   style["title-sep"].toDouble() -
		   br.y()
		   );
}

void PageScene::makePgNoItem() {
  pgNoItem = addText("nnn"
		     /* data->created().toString("MM/dd/yyyy") */,
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
}

void PageScene::positionPgNoItem() {
  QPointF tr = pgNoItem->boundingRect().topRight();
  pgNoItem->setPos(style["margin-left"].toDouble() +
		   style["text-width"].toDouble() +
		   style["margin-right"].toDouble()*.75 -
		   tr.x(),
		   style["margin-top"].toDouble() +
		   style["text-height"].toDouble() +
		   style["pgno-sep"].toDouble() -
		   tr.y()
		   );
}

void PageScene::makeTitleItem() {
  Q_ASSERT(dateItem!=0);
  
  titleItem = addText("Page Title" /* data->title() */,
		      QFont(style["title-font-family"].toString(),
			    style["title-font-size"].toDouble())
		      );
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
  
  sheetNos.clear();
  int sheet = 0;
  double y0 = style["top-margin"].toDouble();
  double y1 = y0 + style["text-height"].toDouble();
  double y = y0;

#if 0
  foreach (BlockItem *bi, blockItems) {
    BlockData *bd = bi->data();
    if (bd->h()==0)
      bd->setH(bi->height());
    if (y!=y0 && y+bd->h()>y1) {
      // start a new sheet for this block
      sheet++;
      y = y0;
    }
    QPointF tl = bi->mapToScene(bi->boundingRect().topLeft());
    bi->moveBy(0, y-tl.y());
    sheetNos.append(sheet);
  }
#endif
  nSheets = sheet+1;

  // contdItem->setVisible(nSheets>1);
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
  Q_ASSERT(titleItem!=0);
  
  iSheet = i;
  if (iSheet>=nSheets)
    iSheet = nSheets-1;
  if (iSheet<0)
    iSheet = 0;

  // Set visibility for all blocks
  int nBlocks = blockItems.size();
  #if 0
  for (int k=0; k<nBlocks; k++)
    blockItems[k]->setVisible(sheetNos[k]==iSheet);
  #endif

  // Remove previous " (n/N)" from title
  QTextCursor c = titleItem->document()->find(QRegExp("\\s+\\(\\d+/\\d+)$"));
  if (!c.isNull())
    c.insertText("");

  if (nSheets>1) {
    // add new " (n/N)" to title
    QTextCursor c(titleItem->document());
    c.movePosition(QTextCursor::End);
    c.insertText(QString(" (%1/%2)").arg(iSheet+1).arg(nSheets));
  }
}
