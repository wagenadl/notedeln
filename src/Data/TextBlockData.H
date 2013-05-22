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

class TextBlockData: public BlockData {
  Q_OBJECT;
public:
  TextBlockData(Data *parent=0);
  virtual ~TextBlockData();
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
protected:
  TextData *text_;
};

#endif
