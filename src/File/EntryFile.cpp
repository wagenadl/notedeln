// File/EntryFile.cpp - This file is part of eln

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

// EntryFile.C

#include "EntryFile.H"
#include "ResManager.H"
#include <QDebug>
#include "Assert.H"
#include "UUID.H"

static QString basicFilename(int pgno, QString uuid) {
  return QString("%1-%2") . arg(pgno, 4, 10, QChar('0')) . arg(uuid);
}

EntryFile *createEntry(QDir const &dir, int n, QObject *parent) {
  QString uuid = UUID::create(32);
  QString fn0 = basicFilename(n, uuid);
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  EntryFile *f = EntryFile::create(pfn, parent);
  ASSERT(f);
  f->data()->setUuid(uuid);
  ResManager *r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(fn0 + ".res");
  r->setRoot(resfn);
  return f;
}


EntryFile *loadEntry(QDir const &dir, int n, QString uuid, QObject *parent) {
  QString fn0 = basicFilename(n, uuid);
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  EntryFile *f = EntryFile::load(pfn, parent);
  if (!f) { // old style simply page number
    fn0 = QString::number(n);
    pfn = dir.absoluteFilePath(fn0 + ".json");
    f = EntryFile::load(pfn, parent);
  }
  ASSERT(f);
  ResManager *r = f->data()->firstChild<ResManager>();
  if (!r)
    r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(fn0 + ".res");
  r->setRoot(resfn);
  return f;
}
