// Data/TitleData.H - This file is part of eln

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

// TitleData.H

#ifndef TITLEDATA_H

#define TITLEDATA_H

#include "Data.H"
#include <QList>
#include "TextData.H"

class TitleData: public Data {
  Q_OBJECT;
public:
  TitleData(Data *parent=0);
  virtual ~TitleData();
  QList<TextData *> versions() const;
  TextData const *current() const;
  TextData *current(); // usu. better to use revise()
  TextData const *orig() const;
  TextData *revise();
  /* revise() creates a new version if previous is more than X hours old.
     For the convenience of TitleItem, the new "current" pointer is always
     the same as the old "current" pointer; the newly created copy is saved
     as the old version.
  */
  bool isDefault() const;
  static QString defaultTitle();  
};

#endif
