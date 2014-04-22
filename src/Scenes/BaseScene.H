// Scenes/BaseScene.H - This file is part of eln

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
public slots:
  void focusTitle(int sheet);
protected slots:
  void titleEdited() {}
  void futileTitleMovement(int, Qt::KeyboardModifiers) {}
protected:
  bool inMargin(QPointF scenePos) const; // true if point is in any margin
  bool inLeftMargin(QPointF scenePos) const;
  bool inRightMargin(QPointF scenePos) const;
  bool inTopMargin(QPointF scenePos) const;
  bool inBottomMargin(QPointF scenePos) const;
  bool inSideMargin(QPointF scenePos) const;
  void setSheetCount(int n);
  void setContInMargin(bool x=true);
  int findSheet(class SheetScene *); // -1 if not found
public: // for SheetScene only
  virtual bool mousePressEvent(QGraphicsSceneMouseEvent *e, SheetScene *s);
  virtual bool keyPressEvent(QKeyEvent *e, SheetScene *s);
protected:
  int nSheets; // number of sheets
  Data *data;
  Style const *style_;
  QList<class SheetScene *> sheets;
  bool contInMargin;
};

#endif
