// File/JSONFile.H - This file is part of eln

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

// JSONFile.H

#ifndef JSONFILE_H

#define JSONFILE_H

#include <QVariant>

namespace JSONFile {
  QVariantMap load(QString fn, bool *ok=0);
  QVariantMap read(QString json, bool *ok=0);
  bool save(QVariantMap const &src, QString fn, bool compact=false);
  QString write(QVariantMap const &src, bool compact=false);
};

#endif
