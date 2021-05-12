// Items/TextItemDoc.h - This file is part of NotedELN

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

// TextItemDoc.H

#ifndef TEXTITEMDOC_H

#define TEXTITEMDOC_H

#include <QObject>

#include <QFont>
#include <QColor>
#include <QRectF>
#include <QMap>
#include "MarkupEdges.h"
#include <QList>
#include <QPair>

class TextItemDoc: public QObject {
  Q_OBJECT;
public:
  static TextItemDoc *create(class TextData *data, QObject *parent=0);
  TextItemDoc(class TextData *data, QObject *parent=0);
  // Properties
  /* Setting a property does _not_ trigger automatic relayout. You must
     call relayout() directly.
  */
  ~TextItemDoc();
  void setFont(QFont const &f);
  QFont font() const;
  QFont font(MarkupData::Style s) const;
  QFont font(MarkupStyles const &s) const;
  void setIndent(double pix);
  double indent() const;
  void setLeftMargin(double pix);
  double leftMargin() const;
  void setRightMargin(double pix);
  double rightMargin() const;
  void setWidth(double pix);
  double width() const;
  void setLineHeight(double pix);
  double lineHeight() const;
  void setY0(double pix);
  double y0() const;
  void setColor(QColor const &c);
  QColor color() const;
  bool isEmpty() const;
  // Other functions
  QChar characterAt(int pos) const;
  QRectF boundingRect() const;
  virtual QRectF tightBoundingRect() const;
  QString text() const;
  QVector<int> lineStarts() const;
  int lineStartFor(int pos) const;
  int lineEndFor(int pos) const;
  int lineFor(int pos) const;
  virtual void relayout(bool preserveWidths=false);
  virtual void partialRelayout(int startOffset, int endOffset);
  void render(class QPainter *p,
              QList<TransientMarkup> tmm=QList<TransientMarkup>()) const;
  virtual int find(QPointF p, bool strict=false) const;
  /* Return offset from graphical position.
     Points outside the bounding rectangle
     get clamped to beginning/end of line for x violations,
     or to start/end of document for y violations.
  */
  QPointF locate(int offset) const; // returns the location of the given
  // offset. The location will be on the baseline of the line of text.
  void insert(int offset, QString text);
  void remove(int offset, int length);
  QPair<int,int> removeWithCombining(int offset, int length);
  // removes combining diacritical marks that are in range as well, returns
  // offset and length of start of removed area
  void swapCase(int offset);
  void transposeCharacters(int offset);
  int find(QString) const; // offset or -1
  void makeWritable();
  virtual int firstPosition() const;
  virtual int lastPosition() const;
  virtual QString selectedText(int start, int end) const;
  virtual double splittableY(double ymax) const;
  /* this will never suggest splitting after the last line, even if
     ymax is very big. */
  virtual double visibleHeight() const;
  void recalculateCharacterWidths();
  virtual void buildLinePos();
  double baselineShift(MarkupStyles const &) const;
  void cautionNoWrite() const;
protected:
  virtual void finalizeConstructor();
signals:
  void contentsChanged(int pos, int nDel, int nIns);
  void markupChanged(MarkupData *md);
protected:
  class TextItemDocData *d;
};

#endif
