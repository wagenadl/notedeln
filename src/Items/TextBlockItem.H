// Items/TextBlockItem.H - This file is part of eln

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

// TextBlockItem.H

#ifndef TEXTBLOCKITEM_H

#define TEXTBLOCKITEM_H

#include "BlockItem.H"
#include "TextItem.H"
#include "FutileMovementInfo.H"
#include "TextBlockData.H"
#include <QTextCursor>
#include <QPointer>

class TICreator {
public:
  virtual ~TICreator();
  virtual class TextItem *create(TextData *data, Item *parent,
				 class QTextDocument *altdoc=0) const;
};

class TextBlockItem: public BlockItem {
  Q_OBJECT;
public:
  TextBlockItem(TextBlockData *data, Item *parent=0,
		TICreator const &tic=TICreator());
  virtual ~TextBlockItem();
  DATAACCESS(TextBlockData);
  QTextDocument *document() const;
  class TextItem *text() const;
  bool isEmpty() const;
  bool lastParIsEmpty() const;
  void dropEmptyLastPar();
  FutileMovementInfo const &lastFutileMovement() const;
  QRectF boundingRect() const;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
  virtual void makeWritable();
  void initializeFormat();
  void setTIFormat(class TextItem *);
  QTextCursor textCursor() const; // looks at focused fragment, if any
  void setTextCursor(QTextCursor c);
public: // splitpar stuff:
  virtual double splittableY(double hmax);
  virtual TextItem *fragment(int fragno);
  virtual int nFragments() const;
  QList<TextItem *> fragments();
  virtual void unsplit();
  virtual void split(QList<double> yy);
signals:
  void futileMovement(); // up key on top line etc
  void sheetRequest(int);
  void multicellular(int, class TextData *);
public slots:
  void sizeToFit();
protected:
  void focusInEvent(QFocusEvent*);
  void mousePressEvent(QGraphicsSceneMouseEvent*);
private slots:
  void futileMovementKey(int, Qt::KeyboardModifiers);
  void ensureVisible(QPointF);
private:
  QList<QPointer<TextItem> > frags; // we do own
  FutileMovementInfo fmi;
  TICreator tic; // this means we cannot split tables for now
};

#endif
