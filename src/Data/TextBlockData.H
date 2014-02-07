// Data/TextBlockData.H - This file is part of eln

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

// TextBlockData.H

#ifndef TEXTBLOCKDATA_H

#define TEXTBLOCKDATA_H

#include "BlockData.H"
#include "TextData.H"
#include <QFlags>

class TextBlockData: public BlockData {
  Q_OBJECT;
  Q_PROPERTY(int ind READ indentationStyle WRITE setIndentationStyle);
public:
  // constructor and destructor
  TextBlockData(Data *parent=0);
  virtual ~TextBlockData();
  // read and write properties
  void setIndented(bool);
  bool indented() const;
  void setDedented(bool);
  bool dedented() const;
  void setDisplayed(bool);
  bool displayed() const;
  // other
  TextData const *text() const;
  TextData *text();
  virtual bool isEmpty() const;
  TextBlockData *split(int pos);
  /* Splits a text block in two pieces, taking care to associate footnotes
     with the correct blocks.
   */
  void join(TextBlockData *); // the other is deleted afterward
protected:
  void loadMore(QVariantMap const &src);
  void setIndentationStyle(int);
  int indentationStyle() const;
protected:
  enum IndentationBits {
    None = 0,
    Indented = 1,
    Displayed = 2,
    Dedented = 4,
  };
  typedef QFlags<IndentationBits> IndentationStyle;
protected:
  TextData *text_;
  int ind;
};

#endif
