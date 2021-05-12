// Data/FootnoteData.H - This file is part of NotedELN

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

// FootnoteData.H

#ifndef FOOTNOTEDATA_H

#define FOOTNOTEDATA_H

#include "TextBlockData.h"

class FootnoteData: public TextBlockData {
  /* We inherit TextBlockData so that a FootnoteItem can be a BlockItem,
     and so that we have y0 and sheet properties.
  */
  Q_OBJECT;
  Q_PROPERTY(QString tag READ tag WRITE setTag)
public:
  FootnoteData(Data *parent=0);
  virtual ~FootnoteData();
  QString tag() const;
  void setTag(QString);
private:
  QString tag_;
};

#endif
