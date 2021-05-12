// App/SceneBank.cpp - This file is part of NotedELN

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

// SceneBank.cpp

#include "SceneBank.h"

#include "Notebook.h"
#include "FrontScene.h"
#include "TOCScene.h"
#include "EntryScene.h"
#include "EntryFile.h"
#include "ElnAssert.h"

SceneBank::SceneBank(Notebook *nb): nb(nb) {
  frontScene_ = new FrontScene(nb, this);

  tocScene_ = new TOCScene(nb->toc(), this);
  tocScene_->populate();
}

SceneBank::~SceneBank() {
  /* Everything gets deleted by Qt automatically. Good magic. */
}

TOCScene *SceneBank::tocScene() {
  return tocScene_;
}

FrontScene *SceneBank::frontScene() {
  return frontScene_;
}

CachedPointer<EntryScene> SceneBank::entryScene(int startPage) {
  if (entryScenes.contains(startPage)) {
    CachedPointer<EntryScene> ptr(entryScenes[startPage]);
    if (ptr)
      return ptr;
  }

  // No cached copy, or deleted cached copy

  /* Ideally, files should be kept in a cache as well. But right now,
     there is no way for the EntryScene CachedPointer to tell that file cache
     when to drop the file. That's because the EntryScene operates on data,
     not files.
     Perhaps the EntryScene could be made to operate on a cached pointer to
     data, so that the data would be auto-deleted when the scene is dropped,
     but how would that information get propagated to the file? Even though
     both Data and DataFile are QObjects, this is not obvious.
     Another possibility would be to create a derivative of cached pointer
     that holds not just its regular payload, but also a second cached pointer
     which gets deleted along with its regular payload. That would work,
     but let me do it later.
  */
  TOCEntry *te = nb->toc()->tocEntry(startPage);
  ASSERT(te);
  CachedEntry entry(nb->entry(startPage));
  ASSERT(entry);
  
  EntryScene *es = new EntryScene(entry, this);
  TOCEntry *nextte = nb->toc()->entryAfter(te);
  if (nextte)
    es->clipPgNoAt(nextte->startPage());
  es->populate();

  CachedPointer<EntryScene> ptr(es);
  entryScenes[startPage] = ptr;
  return ptr;
}
 
