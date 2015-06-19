// Data/MarkupData.H - This file is part of eln

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

// MarkupData.H

#ifndef MARKUPDATA_H

#define MARKUPDATA_H

#include "Data.h"

class MarkupData: public Data {
  /* This is for simple posthoc annotations of text as well as italics,
     scripts, hyperlinks. */
  Q_OBJECT;
  Q_ENUMS(Style)
  Q_PROPERTY(int start READ start WRITE setStart)
  Q_PROPERTY(int end READ end WRITE setEnd)
  Q_PROPERTY(Style style READ style WRITE setStyle)
  Q_ENUMS(Style);
public:
  enum Style {
    // Don't mess with these; they are saved in the .json
    Normal,
    Italic,
    Bold,
    Underline,
    Link,
    FootnoteRef,
    Superscript,
    Subscript,
    StrikeThrough,
    Emphasize,
    Selected, // this is never saved in json; just for MarkupStyles
    DeadLink, // this is never saved in json; just for MarkupStyles
    SearchResult, // this is never saved in json; just for MarkupStyles
  };
public:
  MarkupData(Data *parent=0);
  MarkupData(int start, int end, Style style, Data *parent=0);
  virtual ~MarkupData();
  // read properties
  int start() const;
  int end() const;
  Style style() const;
  // write properties
  void setStart(int);
  void setEnd(int);
  void setStyle(Style);
  // other
  bool isWritable() const;
  bool operator<(MarkupData const &other) const;
  void merge(MarkupData const *other);
  /* only call merge() if mergeable() returns true! */
  bool update(int pos, int del, int ins);
  /* An insertion either immediately before or after our markup will not
     be put inside it. An insertion over an empty markup will go before it.
     Returns true if a change was made. */
  QString text() const; // only works if our parent is TextData
  static QString styleName(Style);
  bool isEmpty() const;
private:
  bool cut(int pos, int len);
  bool insert(int pos, int len);
private:
  int start_;
  int end_;
  Style style_;
  friend bool mergeable(MarkupData const *, MarkupData const *);
};

bool mergeable(MarkupData const *, MarkupData const *);
/* returns true iff other is of the two have same style and overlap */


#endif
