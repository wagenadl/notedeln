// BaseScene.C

#include "BaseScene.H"

#include "Style.H"
#include "Data.H"

#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QDateTime>
#include <QTextDocument>
#include <QDebug>
#include <QKeyEvent>

#include "Notebook.H"
#include "ModSnooper.H"

BaseScene::BaseScene(Data *data, QObject *parent):
  QGraphicsScene(parent),
  data(data) {
  Q_ASSERT(data);
  Notebook *book = data->book();
  Q_ASSERT(book);
  style = &book->style();
  keymods = 0;
  nSheets = 1;
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
	       style->real("page-width"),
	       style->real("page-height"));
  
  setBackgroundBrush(QBrush(style->color("border-color")));
  
  bgItem = addRect(0,
		   0,
 		   style->real("page-width"),
		   style->real("page-height"),
		   QPen(Qt::NoPen),
		   QBrush(style->color("background-color")));

  leftMarginItem = addLine(style->real("margin-left"),
			   0,
			   style->real("margin-left"),
			   height(),
			   QPen(QBrush(style->color("margin-left-line-color")),
				style->real("margin-left-line-width")));
  
  topMarginItem = addLine(0,
			  style->real("margin-top"),
			  width(),
			  style->real("margin-top"),
			  QPen(QBrush(style->color("margin-top-line-color")),
			       style->real("margin-top-line-width")));

}

QString BaseScene::pgNoToString(int n) const {
  return QString::number(n);
}

void BaseScene::makePgNoItem() {
  pgNoItem = addText(pgNoToString(startPage() + iSheet),
		     QFont(style->string("pgno-font-family"),
			   style->real("pgno-font-size")));
  pgNoItem->setDefaultTextColor(style->color("pgno-color"));
  positionPgNoItem();
}


void BaseScene::makeContdItems() {
  contdItem = addText(">",
		      QFont(style->string("contd-font-family"),
			    style->real("contd-font-size")));
  contdItem->setDefaultTextColor(style->color("contd-color"));
  QPointF tr = contdItem->boundingRect().topRight();
  contdItem->setPos(style->real("margin-left") - tr.x(),
		    style->real("margin-top") - tr.y());

  contItem = addText(">",
		     QFont(style->string("contd-font-family"),
			   style->real("contd-font-size")));
  contItem->setDefaultTextColor(style->color("contd-color"));
  QPointF bl = contItem->boundingRect().bottomLeft();
  contItem->setPos(style->real("page-width") - style->real("margin-right")
		   - bl.x(),
		   style->real("page-height") + style->real("margin-bottom")
		   - bl.y());
}

void BaseScene::positionPgNoItem() {
  QPointF tr = pgNoItem->boundingRect().topRight();
  pgNoItem->setPos(style->real("page-width") -
		   style->real("margin-right-over") -
		   tr.x(),
		   style->real("page-height") -
		   style->real("margin-bottom") +
		   style->real("pgno-sep") -
		   tr.y());
}

void BaseScene::makeTitleItem() {
  titleItem = new QGraphicsTextItem(title());
  addItem(titleItem);
  positionTitleItem();
}

void BaseScene::positionTitleItem() {
  titleItem->setTextWidth(style->real("page-width")
			  - style->real("margin-left")
			  - style->real("margin-right"));
  QPointF bl = titleItem->boundingRect().bottomLeft();
  titleItem->setPos(style->real("margin-left") -
		    bl.x(),
		    style->real("margin-top") -
		    style->real("title-sep") -
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
  return sp.x() < style->real("margin-left")
    || sp.x() >= style->real("page-width")
       - style->real("margin-right")
    || sp.y() < style->real("margin-top")
    || sp.y() >= style->real("page-height")
       - style->real("margin-bottom");
}    

void BaseScene::keyChange(int key, int delta) {
  ModSnooper *ms = ModSnooper::instance();
  if (ms && ms->haveState()) {
    // Get info straight from X server
    Qt::KeyboardModifiers m = 0x0;
    if (ms->anyShift())
      m |= Qt::ShiftModifier;
    if (ms->anyControl())
      m |= Qt::ControlModifier;
    if (ms->anyAlt())
      m |= Qt::AltModifier;
    if (m!=keymods) {
      keymods = m;
      emit modifiersChanged(keymods);
    }
  } else {
    // Get info from Qt
    Qt::KeyboardModifiers m = 0x0;
    switch (key) {
    case Qt::Key_Alt: case Qt::Key_AltGr:
      m |= Qt::AltModifier;
    break;
    case Qt::Key_Control:
      m |= Qt::ControlModifier;
    break;
    case Qt::Key_Shift:    
      m |= Qt::ShiftModifier;
    break;
    }
    if (m) {
      if (delta>0)
	keymods |= m;
      else
	keymods &= ~m;
      emit modifiersChanged(keymods);
    }
  }
}

void BaseScene::keyPressEvent(QKeyEvent *e) {
  keyChange(e->key(), +1);
  switch (e->key()) {
  case Qt::Key_PageUp:
    if (!previousSheet())
      emit futileMovement(e);
    e->accept();
    return;
  case Qt::Key_PageDown:
    if (!nextSheet())
      emit futileMovement(e);
    e->accept();
    return;
  }
  QGraphicsScene::keyPressEvent(e);
}

void BaseScene::keyReleaseEvent(QKeyEvent *e) {
  keyChange(e->key(), -1);
  QGraphicsScene::keyReleaseEvent(e);
}

QString BaseScene::title() const {
  return "---";
}

int BaseScene::startPage() const {
  return 1;
}

Qt::KeyboardModifiers BaseScene::keyboardModifiers() const {
  return keymods;
}
