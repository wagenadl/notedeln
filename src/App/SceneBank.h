// App/SceneBank.h - This file is part of eln

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

// SceneBank.H

#ifndef SCENEBANK_H

#define SCENEBANK_H

#include "CachedPointer.h"
#include <QObject>
#include <QMap>

class SceneBank: public QObject {
public:
  SceneBank(class Notebook *nb);
  ~SceneBank();
  Notebook *book() { return nb; }
  class TOCScene *tocScene();
  class FrontScene *frontScene();
  CachedPointer<class EntryScene> entryScene(int startPage);
private:
  Notebook *nb;
  FrontScene *frontScene_;
  TOCScene *tocScene_;
  QMap<int, CachedPointer<EntryScene> > entryScenes;
};

#endif
