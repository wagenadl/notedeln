// File/LateNoteManager.h - This file is part of NotedELN

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

// LateNoteManager.h

#ifndef LATENOTEMANAGER_H

#define LATENOTEMANAGER_H

#include "EntryData.h"
#include "LateNoteData.h"
#include "LateNoteFile.h"
#include <QDir>
#include <QList>

class LateNoteManager: public Data {
public:
  LateNoteManager(QString root, QObject *parent=0);
  virtual ~LateNoteManager() {}
  LateNoteData *newNote(QPointF sp0, QPointF sp1=QPointF());
  QList<LateNoteData *> notes();
  void setBook(class Notebook *);
  virtual void addChild(Data *, ModType mt=UserVisibleMod);
  virtual Data *takeChild(Data *, ModType mt=UserVisibleMod);
  bool needToSave() const;
  bool saveAll() const;
  void ensureLoaded();
private:
  QDir dir;
  bool loaded;
  class Notebook *nb;
  QList<QPointer<LateNoteFile> > files;
};

#endif
