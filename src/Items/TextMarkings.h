// Items/TextMarkings.H - This file is part of eln

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

// TextMarkings.H

#ifndef TEXTMARKINGS_H

#define TEXTMARKINGS_H

#include <QGraphicsObject>
#include <QList>
#include <QTextDocument>
#include "MarkupData.h"
#include "TextData.h"
#include <QSet>

class TextMarkings: public QGraphicsObject {
  Q_OBJECT;
public:
  TextMarkings(TextData *data, class TextItem *parent);
  virtual ~TextMarkings();
  void setSecundary();
  void newMark(MarkupData::Style t, int start, int end);
  void newMark(MarkupData *); // the data object is appropriated into our text
  void deleteMark(MarkupData *); // the data object is deleted from our text 
  TextItem *parent() const;
  QRectF boundingRect() const;
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  void applyAllMarks();
private slots:
  void update(int, int, int);
private:
  class Span;
  Span &insertMark(MarkupData *);
  void applyMark(Span const &, QSet<int> edges=QSet<int>());
private:
  class Span {
  public:
    MarkupData *data;
    QString refText;
  public:
    Span(MarkupData *data, TextMarkings *tm=0);
    bool cut(int pos, int len);
    void insert(TextItem *item, int pos, int len);
    bool update(TextItem *item, int pos, int del, int ins, TextMarkings *);
    // returns true if the span becomes empty
    bool operator<(Span const &other) const;
    void avoidPropagatingStyle(TextItem *item, int pos, int len);
  };
private:
  bool maintainData;
  TextData *data;
  QList<Span> spans; // kept in order of non-decreasing starts
  QMap<class MarkupData *, class HoverRegion *> regions;
  QTextDocument *doc;
};

#endif
