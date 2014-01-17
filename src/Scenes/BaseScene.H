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

#include <QGraphicsScene>

class BaseScene: public QGraphicsScene  {
  Q_OBJECT;
public:
  BaseScene(class Data *data, QObject *parent=0);
  virtual void populate();
  virtual ~BaseScene();
  int sheetCount() const;
  class Style const &style() const;
  virtual bool print(class QPrinter *, class QPainter *,
		     int firstSheet=0, int lastSheet=100000000);
  // returns true if anything printed
  virtual QString toHtml(bool asPage=false);
  int currentSheet() const;
  virtual QGraphicsItem *itemAt(const QPointF &position) const;
  virtual int startPage() const;
  virtual QString title() const;
  virtual QString pgNoToString(int) const;
public slots:
  bool nextSheet(); // true if moved (i.e., if there exists a next sheet)
  bool previousSheet(); // true if moved (i.e., if there exists a prev. sheet)
  virtual void gotoSheet(int i);
  void focusTitle();
protected:
  bool inMargin(QPointF); // true if point is in any margin
  virtual void makeBackground();
  virtual void makeTitleItem();
  virtual void makePgNoItem();
  virtual void makeNofNItem();
  virtual void makeContdItems();
  virtual void positionPgNoItem();
  virtual void positionTitleItem();
protected:
  QGraphicsItem *titleItem;
  QGraphicsTextItem *pgNoItem;
  QGraphicsTextItem *contdItem; // from prev page
  QGraphicsTextItem *contItem; // to next page
  QGraphicsTextItem *nOfNItem;
  QGraphicsLineItem *leftMarginItem;
  QGraphicsLineItem *topMarginItem;
  QGraphicsRectItem *bgItem;
protected:
  int nSheets; // number of sheets
  int iSheet; // current sheet (counting from 0)
  Data *data;
  Style const *style_;
};

#endif
