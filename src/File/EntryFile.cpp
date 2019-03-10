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

#include "EntryFile.h"
#include "ResManager.h"
#include <QDebug>
#include "ElnAssert.h"
#include "UUID.h"

static QString basicFilename(int pgno, QString uuid) {
  return QString("%1-%2") . arg(pgno, 4, 10, QChar('0')) . arg(uuid);
}

EntryFile *createEntry(QDir const &dir, int n, QObject *parent) {
  QString uuid = UUID::create(32);
  QString fn0 = basicFilename(n, uuid);
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  EntryFile *f = EntryFile::create(pfn, parent);
  if (!f)
    return 0;
  f->data()->setUuid(uuid);
  ResManager *r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(fn0 + ".res");
  r->setRoot(resfn);
  return f;
}

static bool removeDir(QDir parent, QString fn) {
  bool ok = true;
  QDir dir(parent.filePath(fn));
  foreach (QFileInfo info,
	   dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden
			     | QDir::AllDirs | QDir::Files)) {
    QString fn = info.filePath();
    if (info.isDir()) 
      ok = removeDir(dir, fn);
    else 
      ok = dir.remove(fn);
    if (!ok)
      return false;
  }
  return parent.rmdir(fn);	
}

bool deleteEntryFile(QDir dir, int n, QString uuid) {
  QString fn0 = basicFilename(n, uuid);
  if (!dir.exists(fn0 + ".json"))
    fn0 = QString::number(n); // quietly revert to old style

  QString jsonfn = fn0 + ".json";
  QString resfn = fn0 + ".res";
  dir.remove(jsonfn + "~");
  removeDir(dir, resfn + "~");
  bool ok = dir.rename(jsonfn, jsonfn + "~");
  dir.rename(resfn, resfn + "~");
  return ok;
}


EntryFile *loadEntry(QDir const &dir, int n, QString uuid, QObject *parent) {
  QString fn0 = basicFilename(n, uuid);
  if (!dir.exists(fn0 + ".json"))
    fn0 = QString::number(n); // quietly revert to old style
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  EntryFile *f = EntryFile::load(pfn, parent);
  if (!f)
    return 0;

  ResManager *r = f->data()->resManager();
  if (!r)
    r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(fn0 + ".res");
  r->setRoot(resfn);
  return f;
}
