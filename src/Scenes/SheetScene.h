// Scenes/SheetScene.h - This file is part of eln

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

// SheetScene.H

#ifndef SHEETSCENE_H

#define SHEETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QDate>
#include <QPointer>

class SheetScene: public QGraphicsScene {
  Q_OBJECT;
public:
  SheetScene(class Style const &s, class BaseScene *parent);
  virtual ~SheetScene();
  void setDate(QDate const &date);
  void setNOfN(int n, int N, bool alwaysShow=false);
  /* n should count 0..N-1 but is printed as 1..N */
  void setTitle(QString const &title);
  void setFancyTitle(class TitleData *data, int sheet,
		     class TextItemDoc *doc=0);
  class TitleItem *fancyTitleItem();
  void setPageNumber(QString n);
  void setContInMargin(bool x=true);
  Style const &style() const { return style_; }
  void setOverlay(QGraphicsObject *);
  void repositionTitle();
  class TextItemDoc *fancyTitleDocument();
  class Mode *mode() const;
  QGraphicsRectItem *backgroundItem() { return bgItem; }
  QGraphicsRectItem *marginItem() { return margItem; }
  static void hideSearchHighlights();
  static void unhideSearchHighlights();
  static bool searchHighlightsVisible();
signals:
  void leaveTitle();
public:
  void setCursors();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *);
  virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *);
  virtual void dropEvent(QGraphicsSceneDragDropEvent *);
  virtual void drawBackground(QPainter *, const QRectF &);
private slots:
  void futileTitleMovement(int, Qt::KeyboardModifiers);
private:
  void makeBackground();
  void repositionDate();
private:
  class BaseScene *base;
  class Style const &style_;
  class QGraphicsTextItem *pgnoItem;
  class QGraphicsItem *titleItem;
  class QGraphicsTextItem *plainTitleItem;
  class TitleItem *fancyTitleItem_;
  class QGraphicsTextItem *contItem;
  class QGraphicsTextItem *contdItem;
  class QGraphicsTextItem *nOfNItem;
  class QGraphicsTextItem *dateItem;
  class QGraphicsLineItem *leftMarginItem;
  class QGraphicsLineItem *topMarginItem;
  class QGraphicsRectItem *bgItem, *margItem;
  bool contInMargin;
  QPointer<QGraphicsObject> overlay;
  static bool &shlvis();
};

#endif
