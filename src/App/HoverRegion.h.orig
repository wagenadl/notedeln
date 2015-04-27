// App/HoverRegion.H - This file is part of eln

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

// HoverRegion.H

#ifndef HOVERREGION_H

#define HOVERREGION_H

#include <QGraphicsObject>

class HoverRegion: public QGraphicsObject {
  Q_OBJECT;
public:
  HoverRegion(class MarkupData *md, class TextItem *item,
	      QGraphicsItem *parent);
  virtual ~HoverRegion();
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  virtual QPainterPath shape() const;
public slots:
  void forgetBounds();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
private slots:
  void downloadFinished();
protected:
  QString refText() const;
  void calcBounds() const;
  void openLink();
  void openArchive();
  void getArchiveAndPreview();
  bool hasArchive() const;
  bool hasPreview() const;
  class Resource *resource() const;
  void activate(QGraphicsSceneMouseEvent *);
  void openPage(bool newView=false);
  QPainterPath const &boundingPath() const { return bounds; }
private:
  MarkupData *md;
  TextItem *ti;
  mutable QPainterPath bounds;
  class PreviewPopper *popper;
  mutable int start;
  mutable int end;
  QString lastRef;
  bool lastRefIsNew;
  bool busy;
};

#endif
