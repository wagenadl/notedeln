// Data/TextData.H - This file is part of NotedELN

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

// TextData.H

#ifndef TEXTDATA_H

#define TEXTDATA_H

#include "Data.h"
#include "MarkupData.h"
#include <QVector>

class TextData: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString text READ text WRITE setText)
public:
  TextData(Data *parent=0);
  virtual ~TextData();
   // read properties
  QString text() const;
  QList<int> paragraphStarts() const;
  virtual QVector<int> const &lineStarts() const;
  // write properties
  virtual void setText(QString const &, bool hushhush=false);
  /* If you change the text, you are responsible for updating the
     markups and the line starts. */
  /* If hushhush is true, the data are not saved. */
  virtual void setLineStarts(QVector<int> const &);
  // other
  bool isEmpty() const;
  QList<MarkupData *> markups() const;
  MarkupData *addMarkup(int start, int end, MarkupData::Style style);
  void addMarkup(MarkupData *); // we become owner
  void deleteMarkup(MarkupData *);
  MarkupData *mergeMarkup(int start, int end, MarkupData::Style style,
			  bool *new_return=0);
  MarkupData *mergeMarkup(MarkupData *, bool *new_return=0);
  /* merge is like add, except that it searches for overlapping
     markups and merges if appropriate. Both forms return a pointer to
     a new markup or to the merge target if a merge was made.
     Optionally, they can indicate whether a new markup was
     created. In either case, ownership of the MarkupData transfers to
     the TextData. */
  MarkupData *markupAt(int start, int end, MarkupData::Style type) const;
  /* markupAt finds the first markup that overlaps partially with [START, END],
     i.e. that starts at or before END and ends at or after START. */
  MarkupData *markupAt(int pos, MarkupData::Style type) const;
  /* markupAt(pos) uses POS for both START and END. */
  MarkupData *markupAt(int start, int end) const;
  /* This overload finds markups regardless of type. */
  MarkupData *markupEdgeIn(int start, int end) const;
  /* Returns a markup that either starts or ends after START and before END. */
  int offsetOfFootnoteTag(QString) const;
  virtual QSet<QString> wordSet() const override;
protected:
  virtual void loadMore(QVariantMap const &) override;
  virtual void saveMore(QVariantMap &) const override;
protected:
  QString text_;
  QVector<int> linestarts;
  mutable QSet<QString> wordset_;
};

#endif
