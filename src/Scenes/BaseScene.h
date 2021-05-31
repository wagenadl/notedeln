// Scenes/BaseScene.H - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// BaseScene.H - Base for TOCScene and EntryScene

#ifndef BASESCENE_H

#define BASESCENE_H

#include <QDate>
#include <QGraphicsScene>
#include <QPointer>

class BaseScene: public QObject  {
  Q_OBJECT;
public:
  BaseScene(class Data *data, QObject *parent=0);
  virtual ~BaseScene();
  int sheetCount() const;
  class Style const &style() const;
  class Notebook *book() const;
  virtual bool print(class QPrinter *, class QPainter *,
		     int firstSheet=0, int lastSheet=100000000);
  // returns true if anything printed
  virtual QGraphicsItem *itemAt(const QPointF &position, int sheet) const;
  virtual int startPage() const;
  virtual QString title() const;
  virtual QDate date() const;
  virtual class TitleData *fancyTitle() const;
  QRectF sceneRect() const;
  virtual QString pgNoToString(int) const;
  virtual void populate();
  void addItem(QGraphicsItem *it, int sheet);
  class SheetScene *sheet(int n, bool autoextend=false);
  class PageView *eventView() const;
  QList<class QGraphicsView *> allViews() const; // all views on this scene
  virtual bool isWritable() const;
public slots:
  virtual void focusTitle(int sheet);
  virtual void focusFirst(int /*sheet*/) {}
protected slots:
  virtual void titleEdited() {}
protected:
  bool inMargin(QPointF scenePos) const; // true if point is in any margin
  bool inLeftMargin(QPointF scenePos) const;
  bool inRightMargin(QPointF scenePos) const;
  bool inTopMargin(QPointF scenePos) const;
  bool inBottomMargin(QPointF scenePos) const;
  bool inSideMargin(QPointF scenePos) const;
  virtual void setSheetCount(int n);
  void setContInMargin(bool x=true);
  int findSheet(class SheetScene *); // -1 if not found
  virtual QList<QGraphicsItem *> printAnnotations(int isheet);
  virtual void waitForLoadComplete() { } // wait for all images to load
public: // for SheetScene only
  virtual bool mousePressEvent(QGraphicsSceneMouseEvent *, SheetScene *);
  virtual bool keyPressEvent(QKeyEvent *, SheetScene *);
  virtual bool dropEvent(QGraphicsSceneDragDropEvent *, SheetScene *);
protected:
  int nSheets; // number of sheets
  class Notebook *book_;
  QList<class SheetScene *> sheets;
  bool contInMargin;
  class QSignalMapper *focusFirstMapper;
};

#endif
