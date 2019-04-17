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

#include "BaseScene.h"

#include "Style.h"
#include "Data.h"
#include "ElnAssert.h"
#include "PageView.h"
#include "SheetScene.h"
#include "TitleItem.h"
#include "SearchDialog.h"

#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QDateTime>
#include <QTextDocument>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QPrinter>
#include <QSignalMapper>

#include "Notebook.h"
#include "BookData.h"

BaseScene::BaseScene(Data *data, QObject *parent):
  QObject(parent) {
  book_ = data->book();
  ASSERT(book_);
  nSheets = 0;
  contInMargin = false;
  focusFirstMapper = new QSignalMapper(this);
  connect(focusFirstMapper, SIGNAL(mapped(int)), SLOT(focusFirst(int)));
}

void BaseScene::populate() {
}

BaseScene::~BaseScene() {
}

QString BaseScene::pgNoToString(int n) const {
  return QString::number(n);
}

bool BaseScene::inLeftMargin(QPointF sp) const {
  return sp.x() < style().real("margin-left");
}

bool BaseScene::inRightMargin(QPointF sp) const {
  return sp.x() >= style().real("page-width") - style().real("margin-right");
}

bool BaseScene::inTopMargin(QPointF sp) const {
  return sp.y() < style().real("margin-top");
}

bool BaseScene::inBottomMargin(QPointF sp) const {
  return sp.y() >= style().real("page-height") - style().real("margin-bottom");
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
  return book()->style();
}

int BaseScene::sheetCount() const {
  return nSheets;
}

void BaseScene::focusTitle(int sheet) {
  ASSERT(sheet>=0 && sheet<nSheets);
  QGraphicsItem *ti = sheets[sheet]->fancyTitleItem();
  if (ti)
    ti->setFocus();
}

bool BaseScene::print(QPrinter *prt, QPainter *p,
		      int firstSheet, int lastSheet) {
  QString phr = SearchDialog::latestPhrase();
  SearchDialog::setLatestPhrase("");

  if (firstSheet<0)
    firstSheet=0;
  if (lastSheet>=nSheets)
    lastSheet = nSheets-1;
  if (lastSheet<nSheets-1
      && pgNoToString(startPage())<"a"
      && pgNoToString(startPage()+lastSheet+1)
      .startsWith(pgNoToString(startPage()+lastSheet)))
    // slightly convoluted way to pick up continuation pages.
    lastSheet = nSheets-1; 
  bool first = true;
  for (int k=firstSheet; k<=lastSheet; k++) {
    if (!first)
      prt->newPage();
    QList<QGraphicsItem *> anno = printAnnotations(k);
    for (auto a: anno)
      sheets[k]->addItem(a);
    sheets[k]->render(p);
    for (auto a: anno)
      sheets[k]->removeItem(a);
    for (auto a: anno)
      delete a;
    first = false;
  }

  if (SearchDialog::latestPhrase().isEmpty())
    SearchDialog::setLatestPhrase(phr);
  
  return !first;
}

QList<QGraphicsItem *> BaseScene::printAnnotations(int /*sheet*/) {
  QList<QGraphicsItem *> lst;
  BookData const *bd = book()->bookData();
  QString au = bd->author().replace("\n", " ");
  QString ti = bd->title().replace("\n", " ");
  if (au.isEmpty() && ti.isEmpty())
    return lst;
  QString sep = (au.isEmpty() || ti.isEmpty()) ? "": ": ";
  QGraphicsTextItem *auti = new QGraphicsTextItem(au + sep + ti);
  QFont f(style().font("pgno-font"));
  f.setStyle(QFont::StyleItalic);
  auti->setFont(f);
  auti->setTextWidth(style().real("page-width")
		     - style().real("margin-left")
		     - style().real("margin-right"));
  auti->setDefaultTextColor(style().color("pgno-color"));
  QPointF tr = auti->boundingRect().topLeft();
  auti->setPos(style().real("margin-left")
		   - tr.x(),
		   style().real("page-height")
		   - style().real("margin-bottom") 
		   + style().real("pgno-sep") 
		   - tr.y());
  lst << auti;
  return lst;
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
      if (k>0)
        if (sheets[0]->fancyTitleItem()->isWritable())
          s->fancyTitleItem()->makeWritable();
      connect(s, SIGNAL(leaveTitle()),
	      focusFirstMapper, SLOT(map()));
      focusFirstMapper->setMapping(s, k);
      connect(s->fancyTitleItem()->document(),
              SIGNAL(contentsChanged(int, int, int)),
	      SLOT(titleEdited()));
      
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

bool BaseScene::dropEvent(QGraphicsSceneDragDropEvent *, SheetScene *) {
  qDebug() << "BaseScene::dropEvent";
  return false;
}

QList<QGraphicsView *> BaseScene::allViews() const {
  QSet<QGraphicsView *> set;
  foreach (SheetScene *s, sheets)
    foreach (QGraphicsView *v, s->views())
      set.insert(v);
  return set.toList();
}

Notebook *BaseScene::book() const {
  return book_;
}

bool BaseScene::isWritable() const {
  return false;
}

