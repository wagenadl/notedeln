// Scenes/BaseScene.cpp - This file is part of eln

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
  iSheet = 0;

  titleItem = pgNoItem = contdItem = contItem = nOfNItem = 0;
  leftMarginItem = topMarginItem = 0;
  bgItem = 0;

  setItemIndexMethod(NoIndex);
}

void BaseScene::populate() {
  makeBackground();
  makePgNoItem();
  makeContdItems();
  makeTitleItem();
  makeNofNItem();
  positionTitleItem();
  positionPgNoItem();
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
  bgItem->setZValue(-100);
  
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
  pgNoItem->setZValue(10); // page number on top
}


void BaseScene::makeContdItems() {
  contdItem = addText(">", style_->font("contd-font"));
  contdItem->setDefaultTextColor(style_->color("contd-color"));
  QPointF tr = contdItem->boundingRect().topRight();
  contdItem->setPos(style_->real("margin-left") - tr.x(),
		    style_->real("margin-top") - tr.y());

  contItem = addText(">",style_->font("contd-font"));
  contItem->setDefaultTextColor(style_->color("contd-color"));
  contItem->setPos(style().real("page-width")
		   - style().real("margin-right-over"),
		   style().real("page-height")
		   - style().real("margin-bottom")
		   + style().real("pgno-sep"));
}

void BaseScene::makeNofNItem() {
  nOfNItem = addText("n/N", style().font("pgno-font"));
  nOfNItem->setDefaultTextColor(style().color("pgno-color"));

  QPointF br = nOfNItem->boundingRect().bottomRight();
  nOfNItem->setPos(style().real("page-width") -
		   style().real("margin-right-over") -
		   br.x(),
		   style().real("margin-top") -
		   style().real("title-sep") -
		   br.y() + 8);
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
		   - style_->real("title-indent")
		   - style_->real("margin-right"));
}

void BaseScene::positionTitleItem() {
  QPointF bl = titleItem->boundingRect().bottomLeft();
  titleItem->setPos(style_->real("margin-left") -
		    bl.x() + style_->real("title-indent"),
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
  if (contdItem)
    contdItem->setVisible(iSheet>0);
  if (contItem)
    contItem->setVisible(iSheet+1 < nSheets);

  // Set page number
  pgNoItem->setPlainText(pgNoToString(startPage() + iSheet));
  positionPgNoItem();

  if (nSheets>1) 
    nOfNItem->setPlainText(QString("(%1/%2)").arg(iSheet+1).arg(nSheets));
  else
    nOfNItem->setPlainText("");
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
  if (lastSheet<nSheets-1
      && pgNoToString(startPage()+lastSheet+1)
      .startsWith(pgNoToString(startPage()+lastSheet)))
    // slightly convoluted way to pick up continuation pages.
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

QGraphicsItem *BaseScene::itemAt(const QPointF &p) const {
  return QGraphicsScene::itemAt(p, QTransform());
}

