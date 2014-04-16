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
	       style_->real("page-height") * nSheets);
  
  setBackgroundBrush(QBrush(style_->color("border-color")));

  for (int n=bgItems.size(); n<nSheets; n++) {
    QGraphicsRectItem *bg = addRect(0,
				    style_->real("page-height")*n,
				    style_->real("page-width"),
				    style_->real("page-height"),
				    QPen(Qt::NoPen),
				    QBrush(style_->color("background-color")));
    bg->setZValue(-100);
    bgItems << bg;
  }

  for (int n=leftMarginItems.size(); n<nSheets; n++) 
    leftMarginItems
      << addLine(style_->real("margin-left")-1,
		 style_->real("page-height")*n,
		 style_->real("margin-left")-1,
		 style_->real("page-height"),
		 QPen(QBrush(style_->color("margin-left-line-color")),
		      style_->real("margin-left-line-width")));
  
  for (int n=topMarginItems.size(); n<nSheets; n++) 
    topMarginItems
      << addLine(0,
		 style_->real("page-height")*n+style_->real("margin-top"),
		 width(),
		 style_->real("page-height")*n+style_->real("margin-top"),
		 QPen(QBrush(style_->color("margin-top-line-color")),
		      style_->real("margin-top-line-width")));
}

QString BaseScene::pgNoToString(int n) const {
  return QString::number(n);
}

void BaseScene::makePgNoItem() {
  for (int n=pgNoItems.size(); n<nSheets; n++) {
    QGraphicsTextItem *pgno = addText(pgNoToString(startPage() + n),
				      style_->font("pgno-font"));
    pgno->setDefaultTextColor(style_->color("pgno-color"));
    pgno->setZValue(10); // page number on top
    pgno->setPlainText(pgNoToString(startPage() + n));
    pgNoItems << pgno;
  }
}


void BaseScene::makeContdItems() {
  for (int n=contdItems.size(); n<nSheets-1; n++) {
    QGraphicsTextItem *c = addText(">", style_->font("contd-font"));
    c->setDefaultTextColor(style_->color("contd-color"));
    QPointF tr = c->boundingRect().topRight();
    c->setPos(style_->real("margin-left") - tr.x(),
	      style_->real("page-height")*n
	      + style_->real("margin-top") - tr.y());
    contdItems << c;
  }

  for (int n=contItems.size(); n<nSheets; n++) {
    if (n==0) {
      contItems << 0;
    } else {
      QGraphicsTextItem *c = addText(">",style_->font("contd-font"));
      c->setDefaultTextColor(style_->color("contd-color"));
      c->setPos(style().real("page-width")
		- style().real("margin-right-over"),
		style().real("page-height")*(n+1)
		- style().real("margin-bottom")
		+ style().real("pgno-sep"));
         contItems << c;
    }
  }
}

void BaseScene::makeNofNItem() {
  for (int n=nOfNItems.size(); n<nSheets; n++) {
    QGraphicsTextItem *nofn = addText("n/N", style().font("pgno-font"));
    nofn->setDefaultTextColor(style().color("pgno-color"));
    QPointF br = nofn->boundingRect().bottomRight();
    nofn->setPos(style().real("page-width") -
		 style().real("margin-right-over") -
		 br.x(),
		 style().real("page-height")*n
		 + style().real("margin-top") -
		 style().real("title-sep") -
		 br.y() + 8);
    nOfNItems << nofn;
  }
  if (nSheets==1)
    nOfNItems[0]->setPlainText("");
  else
    for (int n=0; n<nSheets; n++) 
      nOfNItems[n]->setPlainText(QString("(%1/%2)").arg(n+1).arg(nSheets));
}  

void BaseScene::positionPgNoItem() {
  for (int n=0; n<nSheets; n++) {
    QPointF tr = pgNoItems[n]->boundingRect().topRight();
    pgNoItems[n]->setPos(style_->real("page-width") -
			 style_->real("margin-right-over") -
			 tr.x(),
			 style_->real("page-height")*(n+1) -
			 style_->real("margin-bottom") +
			 style_->real("pgno-sep") -
			 tr.y());
  }
}

void BaseScene::makeTitleItem() {
  for (int n=titleItems.size(); n<nSheets; n++) {
    QGraphicsTextItem *tt = new QGraphicsTextItem(title());
    titleItems << tt;
    tt->setFont(style().font("title-font"));
    tt->setDefaultTextColor(style().color("title-color"));
    addItem(tt);
    tt->setTextWidth(style_->real("page-width")
		     - style_->real("margin-left")
		     - style_->real("title-indent")
		     - style_->real("margin-right"));
  }
}

void BaseScene::positionTitleItem() {
  for (int n=0; n<nSheets; n++) {
    QPointF bl = titleItems[n]->boundingRect().bottomLeft();
    titleItems[n]->setPos(style_->real("margin-left") -
		      bl.x() + style_->real("title-indent"),
		      style_->real("page-height")*n -
		      style_->real("margin-top") -
		      style_->real("title-sep") -
		      bl.y());
  }
}

  
bool BaseScene::inMargin(QPointF sp) {
  double y = sp.y();
  double h = style_->real("page-height");
  while (y>=h)
    y-=h;
    
  return sp.x() < style_->real("margin-left")
    || sp.x() >= style_->real("page-width") - style_->real("margin-right")
    || y < style_->real("margin-top")
    || y >= h - style_->real("margin-bottom");
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
  ASSERT(titleItems[0]);
  titleItems[0]->setFocus();
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
  bool first = true;
  for (int k=firstSheet; k<=lastSheet; k++) {
    if (!first)
      prt->newPage();
    render(p, QRectF(), rectForSheet(k));
    first = false;
  }
  return !first;
}

QGraphicsItem *BaseScene::itemAt(const QPointF &p) const {
  return QGraphicsScene::itemAt(p, QTransform());
}

void BaseScene::setOverlay(SceneOverlay *ovr) {
  if (overlay) {
    delete overlay;
    overlay = 0;
  }
  overlay = ovr;
  if (overlay)
    addItem(ovr);
}

