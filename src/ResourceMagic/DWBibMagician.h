// ResourceMagic/DWBibMagician.H - This file is part of eln

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

// DWBibMagician.H

#ifndef DWBIBMAGICIAN_H

#define DWBIBMAGICIAN_H

#include "Magician.h"

class DWBibMagician: public Magician {
public:
  DWBibMagician();
  DWBibMagician(class Style const &);
  virtual bool matches(QString) const;
  virtual QUrl objectUrl(QString) const;
  virtual QString title(QString) const; 
  //  virtual bool keepAlways(QString) const;
private:
  Style const &style;
};

#endif
