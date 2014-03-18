// File/EntryFile.H - This file is part of eln

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

// EntryFile.H

#ifndef ENTRYFILE_H

#define ENTRYFILE_H

#include "DataFile.H"
#include "EntryData.H"
#include <QDir>

typedef DataFile<EntryData> EntryFile;
EntryFile *createEntry(QDir const &dir, int n, QObject *parent=0);
EntryFile *loadEntry(QDir const &dir, int n, QString uuid, QObject *parent=0);

#endif
