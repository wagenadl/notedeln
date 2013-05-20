// webgrab/Options.H - This file is part of eln

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

// Options.H

#ifndef OPTIONS_H

#define OPTIONS_H

#include <QStringList>

class Options {
public:
  Options(int &argc, char **&argv);
  bool paginate;
  int imSize;
  QString url;
  QStringList out;
  void usage();
  
};

#endif
