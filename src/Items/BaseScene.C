// BaseScene.C

#include "BaseScene.H"

#include "Style.H"
#include "Data.H"
#include "Assert.H"
#include "PageView.H"

#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QDateTime>
#include <QTextDocument>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QPrinter>

#include "Notebook.H"

BaseScene::BaseScene(Data *data, QObject *parent):
  QGraphicsScene(parent),
  data(data) {
  ASSERT(data);
  Notebook *book = data->book();
  ASSERT(book);
  style_ = &book->style();
  nSheets = 1;

  titleItem = pgNoItem = contdItem = contItem = 0;
  leftMarginItem = topMarginItem = 0;
  bgItem = 0;
  // must populate!
}

void BaseScene::populate() {
  makeBackground();
  makePgNoItem();
  makeContdItems();
  makeTitleItem();
  gotoSheet(0);
}

BaseScene::~BaseScene() {
}

void BaseScene::makeBackground() {
  setSceneRect(0,
	       0,
	       style_->real("page-width"),
	       style_->real("page-height"));
  
  setBackgroundBrush(QBrush(style_->color("border-color")));
  
  bgItem = addRect(0,
		   0,
 		   style_->real("page-width"),
		   style_->real("page-height"),
		   QPen(Qt::NoPen),
		   QBrush(style_->color("background-color")));

  leftMarginItem = addLine(style_->real("margin-left")-1,
			   0,
			   style_->real("margin-left")-1,
			   height(),
			   QPen(QBrush(style_->color("margin-left-line-color")),
				style_->real("margin-left-line-width")));
  
  topMarginItem = addLine(0,
			  style_->real("margin-top"),
			  width(),
			  style_->real("margin-top"),
			  QPen(QBrush(style_->color("margin-top-line-color")),
			       style_->real("margin-top-line-width")));

}

QString BaseScene::pgNoToString(int n) const {
  return QString::number(n);
}

void BaseScene::makePgNoItem() {
  pgNoItem = addText(pgNoToString(startPage() + iSheet),
		     style_->font("pgno-font"));
  pgNoItem->setDefaultTextColor(style_->color("pgno-color"));
  positionPgNoItem();
}


void BaseScene::makeContdItems() {
  contdItem = addText(">", style_->font("contd-font"));
  contdItem->setDefaultTextColor(style_->color("contd-color"));
  QPointF tr = contdItem->boundingRect().topRight();
  contdItem->setPos(style_->real("margin-left") - tr.x(),
		    style_->real("margin-top") - tr.y());

  contItem = addText(">",style_->font("contd-font"));
  contItem->setDefaultTextColor(style_->color("contd-color"));
  QPointF bl = contItem->boundingRect().bottomLeft();
  contItem->setPos(style_->real("page-width") - style_->real("margin-right")
		   - bl.x(),
		   style_->real("page-height") - style_->real("margin-bottom")
		   - bl.y());
}

void BaseScene::positionPgNoItem() {
  QPointF tr = pgNoItem->boundingRect().topRight();
  pgNoItem->setPos(style_->real("page-width") -
		   style_->real("margin-right-over") -
		   tr.x(),
		   style_->real("page-height") -
		   style_->real("margin-bottom") +
		   style_->real("pgno-sep") -
		   tr.y());
}

void BaseScene::makeTitleItem() {
  QGraphicsTextItem *tt = new QGraphicsTextItem(title());
  titleItem = tt;
  tt->setFont(style().font("title-font"));
  tt->setDefaultTextColor(style().color("title-color"));
  addItem(titleItem);
  tt->setTextWidth(style_->real("page-width")
		   - style_->real("margin-left")
		   - style_->real("title-sep")
		   - style_->real("margin-right"));
  positionTitleItem();
}

void BaseScene::positionTitleItem() {
  QPointF bl = titleItem->boundingRect().bottomLeft();
  titleItem->setPos(style_->real("margin-left") -
		    bl.x() + style_->real("title-sep"),
		    style_->real("margin-top") -
		    style_->real("title-sep") -
		    bl.y());
}

bool BaseScene::previousSheet() {
  if (iSheet>0) {
    gotoSheet(iSheet-1);
    return true;
  } else {
    return false;
  }
}

bool BaseScene::nextSheet() {
  if (iSheet<nSheets-1) {
    gotoSheet(iSheet+1);
    return true;
  } else {
    return false;
  }
}

void BaseScene::gotoSheet(int i) {
  iSheet = i;
  if (iSheet>=nSheets)
    iSheet = nSheets-1;
  if (iSheet<0)
    iSheet = 0;

  // Set visibility of continuation markers
  contdItem->setVisible(iSheet>0);
  contItem->setVisible(iSheet+1 < nSheets);

  // Set page number
  pgNoItem->setPlainText(pgNoToString(startPage() + iSheet));
  positionPgNoItem();
}
  
bool BaseScene::inMargin(QPointF sp) {
  return sp.x() < style_->real("margin-left")
    || sp.x() >= style_->real("page-width")
       - style_->real("margin-right")
    || sp.y() < style_->real("margin-top")
    || sp.y() >= style_->real("page-height")
       - style_->real("margin-bottom");
}    

QString BaseScene::title() const {
  return "---";
}

int BaseScene::startPage() const {
  return 1;
}

Style const &BaseScene::style() const {
  return *style_;
}

int BaseScene::sheetCount() const {
  return nSheets;
}

void BaseScene::focusTitle() {
  ASSERT(titleItem);
  titleItem->setFocus();
}

bool BaseScene::print(QPrinter *prt, QPainter *p,
		      int firstSheet, int lastSheet) {
  if (firstSheet<0)
    firstSheet=0;
  if (lastSheet>=nSheets)
    lastSheet = nSheets-1;
  int oldSheet = iSheet;
  bool first = true;
  for (int k=firstSheet; k<=lastSheet; k++) {
    if (!first)
      prt->newPage();
    if (iSheet != k)
      gotoSheet(k);
    render(p);
    first = false;
  }
  if (iSheet != oldSheet)
    gotoSheet(iSheet);
  return !first;
}

int BaseScene::currentSheet() const {
  return iSheet;
}
