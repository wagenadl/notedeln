// File/LateNoteFile.cpp - This file is part of NotedELN

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

// LateNoteFile.cpp

#include "LateNoteFile.h"

#include <QDebug>
#include "ElnAssert.h"
#include "UUID.h"
#include "ResManager.h"

static QString basicFilename(QString uuid) {
  return QString("%1") . arg(uuid);
}

LateNoteFile *createLateNoteFile(QDir const &dir, QObject *parent) {
  QString uuid = UUID::create(32);
  QString fn0 = basicFilename(uuid);
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  LateNoteFile *f = LateNoteFile::create(pfn, parent);
  if (!f)
    return 0;
  f->data()->setUuid(uuid);

  ResManager *r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(fn0 + ".res");
  r->setRoot(resfn);

  return f;
}

LateNoteFile *loadLateNoteFile(QDir const &dir, QString uuid, QObject *parent) {
  QString fn0 = basicFilename(uuid);
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  LateNoteFile *f = LateNoteFile::load(pfn, parent);
  if (!f)
    return 0;

  ResManager *r = f->data()->resManager();
  if (!r)
    r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(fn0 + ".res");
  r->setRoot(resfn);

  return f;
}

bool deleteLateNoteFile(QDir dir, QString uuid) {
  QString fn0 = basicFilename(uuid);
  QString jsonfn = fn0 + ".json";
  dir.remove(jsonfn + "~");
  bool ok = dir.rename(jsonfn, jsonfn + "~");
  return ok;
}

