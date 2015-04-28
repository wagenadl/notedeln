// Data/TextData.H - This file is part of eln

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

// TextData.H

#ifndef TEXTDATA_H

#define TEXTDATA_H

#include "Data.h"
#include "MarkupData.h"

class TextData: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString text READ text WRITE setText)
public:
  TextData(Data *parent=0);
  virtual ~TextData();
   // read properties
  QString text() const;
  // write properties
  void setText(QString const &);
  /* If you change the text in the presence of markups, you are responsible
     for updating the markups */
  // other
  bool isEmpty() const;
  QList<MarkupData *> markups() const;
  MarkupData *addMarkup(int start, int end, MarkupData::Style style);
  void addMarkup(MarkupData *);
  void deleteMarkup(MarkupData *);
  int offsetOfFootnoteTag(QString) const;
protected:
  QString text_;
};

#endif
