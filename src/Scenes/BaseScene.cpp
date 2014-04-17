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
#include "SheetScene.H"

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
  QObject(parent),
  data(data) {
  ASSERT(data);
  Notebook *book = data->book();
  ASSERT(book);
  style_ = &book->style();
  nSheets = 0;
  contInMargin = false;
}

void BaseScene::populate() {
}

BaseScene::~BaseScene() {
}

QString BaseScene::pgNoToString(int n) const {
  return QString::number(n);
}

bool BaseScene::inLeftMargin(QPointF sp) const {
  return sp.x() < style_->real("margin-left");
}

bool BaseScene::inRightMargin(QPointF sp) const {
  return sp.x() >= style_->real("page-width") - style_->real("margin-right");
}

bool BaseScene::inTopMargin(QPointF sp) const {
  return sp.y() < style_->real("margin-top");
}

bool BaseScene::inBottomMargin(QPointF sp) const {
  return sp.y() >= style_->real("page-height") - style_->real("margin-bottom");
}

bool BaseScene::inSideMargin(QPointF sp) const {
  return inLeftMargin(sp) || inRightMargin(sp);
}

bool BaseScene::inMargin(QPointF sp) const {
  return inSideMargin(sp) || inTopMargin(sp) || inBottomMargin(sp);
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
  qDebug() << "BaseScene::focusTitle NYI";
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
    sheets[k]->render(p);
    first = false;
  }
  return !first;
}

QGraphicsItem *BaseScene::itemAt(const QPointF &p, int sheet) const {
  if (sheet>=0 && sheet<nSheets)
    return sheets[sheet]->itemAt(p, QTransform());
  else
    return 0;
}

class TitleData *BaseScene::fancyTitle() const {
  return 0;
}

QDate BaseScene::date() const {
  return QDate();
}

void BaseScene::setSheetCount(int n) {
  ASSERT(n>=0);

  // drop old sheets
  while (sheets.size()>n) 
    sheets.takeLast()->deleteLater();

  // add new sheets
  while (sheets.size()<n) {
    int k = sheets.size();
    SheetScene *s = new SheetScene(style(), this);
    if (fancyTitle()) {
      if (k==0)
	s->setFancyTitle(fancyTitle(), 0);
      else
	s->setFancyTitle(fancyTitle(), k, sheets[0]->fancyTitleDocument());
    } else {
      s->setTitle(title());
    }
    s->setPageNumber(pgNoToString(startPage() + k));
    s->setDate(date());
    s->setContInMargin(contInMargin);
    sheets << s;
  }

  nSheets = n;
  
  for (int k=0; k<n; k++)
    sheets[k]->setNOfN(k, n);
}


void BaseScene::setContInMargin(bool x) {
  contInMargin = x;
  foreach (SheetScene *s, sheets)
    s->setContInMargin(x);
}

QRectF BaseScene::sceneRect() const {
  if (sheets.isEmpty())
    return QRectF();
  else
    return sheets[0]->sceneRect();
}

void BaseScene::addItem(QGraphicsItem *it, int n) {
  sheet(n)->addItem(it);
}

SheetScene *BaseScene::sheet(int n, bool autoextend) {
  ASSERT(n>=0);
  if (autoextend) 
    if (n>=nSheets)
      setSheetCount(n+1);
  ASSERT(n<nSheets);
  return sheets[n];
}

int BaseScene::findSheet(SheetScene *ss) {
  for (int n=0; n<nSheets; n++)
    if (sheets[n]==ss)
      return n;
  return -1;
}

bool BaseScene::mousePressEvent(QGraphicsSceneMouseEvent *, SheetScene *) {
  return false;
}

bool BaseScene::keyPressEvent(QKeyEvent *, SheetScene *) {
  return false;
}
