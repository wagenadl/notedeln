// Items/TextBlockItem.H - This file is part of NotedELN

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

// TextBlockItem.H

#ifndef TEXTBLOCKITEM_H

#define TEXTBLOCKITEM_H

#include "BlockItem.h"
#include "TextItem.h"
#include "FutileMovementInfo.h"
#include "TextBlockData.h"
#include "TextCursor.h"
#include <QPointer>

class TICreator {
public:
  virtual ~TICreator();
  virtual class TextItem *create(TextData *data, Item *parent,
				 class TextItemDoc *altdoc=0) const;
};

class TextBlockItem: public BlockItem {
  Q_OBJECT;
public:
  TextBlockItem(TextBlockData *data, Item *parent=0,
		TICreator const &tic=TICreator());
  virtual ~TextBlockItem();
  DATAACCESS(TextBlockData);
  TextItemDoc *document() const;
  class TextItem *text() const;
  bool isEmpty() const;
  bool lastParIsEmpty() const;
  void dropEmptyLastPar();
  FutileMovementInfo const &lastFutileMovement() const;
  QRectF boundingRect() const override;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w) override;
  virtual void makeWritable() override;
  void initializeFormat();
  void setTIFormat(class TextItem *);
  TextCursor textCursor() const; // looks at focused fragment, if any
  void setTextCursor(TextCursor c);
  int findFragmentForPhrase(QString phrase) const; // -1 if not found
public: // splitpar stuff:
  virtual double visibleHeight() const override;
  virtual double splittableY(double hmax) const override;
  virtual TextItem *fragment(int fragno) override;
  virtual int nFragments() const override;
  QList<TextItem *> fragments();
  virtual void unsplit() override;
  virtual void split(QList<double> yy) override;
signals:
  void futileMovement(); // up key on top line etc
  void sheetRequest(int);
  void multicellular(int, TextData *);
  void multicellularpaste(TextData *, QString);
public slots:
  void sizeToFit(bool shrink=true) override;
  void muckWithIndentation(TextCursor);
  void testmulticellularpaste(TextData *, QString);
protected:
  void focusInEvent(QFocusEvent *) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *) override;
private slots:
  void futileMovementKey(int, Qt::KeyboardModifiers);
  void ensureVisible(TextCursor, QPointF);
private:
  QList<QPointer<TextItem> > frags; // we do own
  FutileMovementInfo fmi;
  TICreator tic; // this means we cannot split tables for now
};

#endif
