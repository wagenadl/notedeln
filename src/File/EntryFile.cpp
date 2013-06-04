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

EntryFile *createPage(QDir const &dir, int n, QObject *parent) {
  QString pfn = dir.absoluteFilePath(QString::number(n) + ".json");
  EntryFile *f = EntryFile::create(pfn, parent);
  ASSERT(f);
  ResManager *r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(QString::number(n) + ".res");
  r->setRoot(resfn);
  return f;
}

EntryFile *loadPage(QDir const &dir, int n, QObject *parent) {
  QString pfn = dir.absoluteFilePath(QString::number(n) + ".json");
  EntryFile *f = EntryFile::load(pfn, parent);
  ASSERT(f);
  ResManager *r = f->data()->firstChild<ResManager>();
  if (!r)
    r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(QString::number(n) + ".res");
  r->setRoot(resfn);
  return f;
}
