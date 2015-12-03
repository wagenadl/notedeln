// Scenes/SheetScene.cpp - This file is part of eln

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

// SheetScene.cpp

#include "SheetScene.h"
#include "BaseScene.h"
#include "Style.h"
#include "TextData.h"
#include "TitleItem.h"
#include "PageView.h"
#include "Cursors.h"
#include "EventView.h"
#include "Notebook.h"

#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QPainter>
#include <QTextDocument>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>

SheetScene::SheetScene(class Style const &s, BaseScene *parent):
  QGraphicsScene(parent), base(parent), style_(s) {
  pgnoItem = 0;
  nOfNItem = 0;
  titleItem = 0;
  plainTitleItem = 0;
  fancyTitleItem_ = 0;
  contItem = 0;
  contdItem = 0;
  dateItem = 0;
  leftMarginItem = 0;
  topMarginItem = 0;
  //  bgItem = 0;
  contInMargin = false;
  
  setItemIndexMethod(NoIndex);
  makeBackground();
}

SheetScene::~SheetScene() {
}

void SheetScene::makeBackground() {
  setSceneRect(0, 0,
	       style_.real("page-width"), style_.real("page-height"));

  margItem
    = addRect(0, 0, style_.real("page-width"), style_.real("page-height"),
	      QPen(Qt::NoPen), QBrush(style_.color("background-color"))); 
  margItem->setZValue(-100);
  
  bgItem
    = addRect(style_.real("margin-left"), style_.real("margin-top"),
	      style_.real("page-width")
	      - style_.real("margin-left") - style_.real("margin-right"),
	      style_.real("page-height")
	      - style_.real("margin-top") - style_.real("margin-bottom"),
	      QPen(Qt::NoPen), QBrush(style_.color("background-color"))); 
  bgItem->setZValue(-90);
   
  leftMarginItem
    = addLine(style_.real("margin-left")-1, 0,
	      style_.real("margin-left")-1, style_.real("page-height"),
	      QPen(QBrush(style_.color("margin-left-line-color")),
		   style_.real("margin-left-line-width")));

    topMarginItem
      = addLine(0, style_.real("margin-top"),
		width(), style_.real("margin-top"),
		QPen(QBrush(style_.color("margin-top-line-color")),
		     style_.real("margin-top-line-width")));
}
  
void SheetScene::setDate(QDate const &date) {
  if (!dateItem) {
    dateItem = addText("-", style_.font("date-font"));
    dateItem->setDefaultTextColor(style_.color("date-color"));
  }
  if (date.isNull()) {
    dateItem->hide();
  } else {
    dateItem->setPlainText(date.toString(style_.string("date-format")));
    dateItem->show();
    repositionDate();
  }
}

void SheetScene::repositionDate() {
  QPointF br = dateItem->boundingRect().bottomRight();
  if (nOfNItem && nOfNItem->isVisible())
    dateItem->setPos(nOfNItem->sceneBoundingRect().topRight()
		     - br + QPointF(0, 8));
  else 
    dateItem->setPos(style().real("page-width") 
                     - style().real("margin-right-over") 
                     - br.x(),
                     style().real("margin-top") 
                     - style().real("title-sep") 
                     - br.y());
  repositionTitle();
}

void SheetScene::setNOfN(int n, int N, bool always) {
  if (!nOfNItem) {
     nOfNItem = addText("n/N", style_.font("pgno-font"));
     nOfNItem->setDefaultTextColor(style_.color("pgno-color"));
  }
  if (N>1 || always) {
    nOfNItem->setPlainText(QString("(%1/%2)").arg(n+1).arg(N));
    QPointF br = nOfNItem->boundingRect().bottomRight();
    nOfNItem->setPos(style_.real("page-width") 
		     - style_.real("margin-right-over") 
		     - br.x(),
		     style_.real("margin-top") 
		     - style_.real("title-sep") 
		     - br.y() + 8);
    nOfNItem->show();
  } else {
    nOfNItem->hide();
  }

  repositionDate();

  if (n>=1) {
    // this is a continuation page
    if (!contdItem) {
      contdItem = addText(">", style_.font("contd-font"));
      contdItem->setDefaultTextColor(style_.color("contd-color"));
      QPointF tr = contdItem->boundingRect().topRight();
      double x = contInMargin ? 4
	: style_.real("margin-left") - tr.x();
      contdItem->setPos(x, style_.real("margin-top") - tr.y());
    }
    contdItem->show();
  } else {
    if (contdItem) 
      contdItem->hide();
  }
  if (n<N-1) {
    // this is not the last page
    if (!contItem) {
      contItem = addText(">",style_.font("contd-font"));
      contItem->setDefaultTextColor(style_.color("contd-color"));
      contItem->setPos(style_.real("page-width")
		       - style_.real("margin-right-over"),
		       style_.real("page-height")
		       - style_.real("margin-bottom")
		       + style_.real("pgno-sep"));
    }
    contItem->show();
  } else {
    if (contItem)
      contItem->hide();
  }
}

void SheetScene::setTitle(QString const &title) {
  if (fancyTitleItem_)
    fancyTitleItem_->deleteLater();
  fancyTitleItem_ = 0;
  if (!plainTitleItem) {
    plainTitleItem = addText("", style_.font("title-font"));
    plainTitleItem->setDefaultTextColor(style_.color("title-color"));
  }
  plainTitleItem->setPlainText(title);
  titleItem = plainTitleItem;
  repositionTitle();
}

void SheetScene::repositionTitle() {
  double l = style_.real("margin-left") + style_.real("title-indent") + 4;
  double r = style_.real("page-width") - style_.real("margin-right");
  if (dateItem) {
    double dateX = dateItem->mapToScene(dateItem->boundingRect().topLeft()).x();
    dateX -= 5; // should be in style, I guess
    if (r>dateX)
      r = dateX;
  }
  double w = r - l;
  if (fancyTitleItem_)
    fancyTitleItem_->setTextWidth(w, false);
  if (plainTitleItem)
    plainTitleItem->setTextWidth(w);    
  QPointF bl = (fancyTitleItem_
		? fancyTitleItem_->netBounds() 
		: titleItem->boundingRect())
    .bottomLeft();
  titleItem->setPos(l - bl.x(),
                    style_.real("margin-top")
                    - style_.real("title-sep") - bl.y() - 4);
}

TitleItem *SheetScene::fancyTitleItem() {
  return fancyTitleItem_;
}

void SheetScene::setFancyTitle(TitleData *data, int sheet,
			       TextItemDoc *doc) {
  if (plainTitleItem)
    plainTitleItem->deleteLater();
  plainTitleItem = 0;
  if (fancyTitleItem_)
    fancyTitleItem_->deleteLater();
  fancyTitleItem_ = new TitleItem(data, sheet, doc);
  connect(fancyTitleItem_,
	  SIGNAL(futileMovementKey(int, Qt::KeyboardModifiers)),
	  SLOT(futileTitleMovement(int, Qt::KeyboardModifiers)));
  addItem(fancyTitleItem_);
  titleItem = fancyTitleItem_;
  repositionTitle();
}

TextItemDoc *SheetScene::fancyTitleDocument() {
  if (fancyTitleItem_)
    return fancyTitleItem_->document();
  else
    return 0;
}      

void SheetScene::setPageNumber(QString n) {
  if (!pgnoItem) {
   pgnoItem = addText("", style_.font("pgno-font"));
   pgnoItem->setDefaultTextColor(style_.color("pgno-color"));
   pgnoItem->setZValue(10); // page number on top
  }

  pgnoItem->setPlainText(n);
  QPointF tr = pgnoItem->boundingRect().topRight();
  pgnoItem->setPos(style_.real("page-width") 
		   - style_.real("margin-right-over") 
		   - tr.x(),
		   style_.real("page-height")
		   - style_.real("margin-bottom") 
		   + style_.real("pgno-sep") 
		   - tr.y());
}

void SheetScene::setContInMargin(bool x) {
  contInMargin = x;
}

void SheetScene::setOverlay(QGraphicsObject *gi) {
  if (overlay)
    delete overlay;
  overlay = gi;
  if (overlay)
    addItem(overlay);
}

void SheetScene::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (base && base->mousePressEvent(e, this))
    return;
  QGraphicsScene::mousePressEvent(e);
}

void SheetScene::dragEnterEvent(QGraphicsSceneDragDropEvent *e) {
  e->acceptProposedAction(); // we pretend we can accept anything
}

void SheetScene::dropEvent(QGraphicsSceneDragDropEvent *e) {
  if (base && base->dropEvent(e, this))
    return;
  QGraphicsScene::dropEvent(e);
}

void SheetScene::keyPressEvent(QKeyEvent *e) {
  if (base && base->keyPressEvent(e, this))
    return;
  QGraphicsScene::keyPressEvent(e);      
}

void SheetScene::drawBackground(QPainter *p, const QRectF &r) {
  QBrush border(style().color("border-color"));
  QBrush background(style().color("background-color"));
  p->setPen(Qt::NoPen);
  p->setBrush(border);
  p->drawRect(r);
  p->setBrush(background);
  p->drawRect(sceneRect());
}

Mode *SheetScene::mode() const {
  ASSERT(base);
  return base->book()->mode();
}

void SheetScene::futileTitleMovement(int key, Qt::KeyboardModifiers) {
  switch (key) {
  case Qt::Key_Enter:
  case Qt::Key_Return:
  case Qt::Key_Down:
    emit leaveTitle();
    break;
  default:
    break;
  }
}

void SheetScene::setCursors() {
  Qt::CursorShape marginShape = Item::defaultCursorShape();
  Qt::CursorShape backgroundShape = Item::defaultCursorShape();
  Mode *m = mode();
  if (m) {
    switch (m->mode()) {
    case Mode::Type:
      backgroundShape = Qt::IBeamCursor;
      break;
    case Mode::Mark: case Mode::Freehand:
      backgroundShape = Qt::CrossCursor;
      break;
    case Mode::Annotate:    
      marginShape = backgroundShape = Qt::CrossCursor;
      break;
    case Mode::Highlight: case Mode::Strikeout: case Mode::Plain:
      break;
    default:
      break;
    }
  }  

  marginItem()->setCursor(Cursors::refined(marginShape));
  backgroundItem()->setCursor(Cursors::refined(backgroundShape));
}  
