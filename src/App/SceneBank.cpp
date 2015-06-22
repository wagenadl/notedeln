// SceneBank.cpp

#include "SceneBank.h"

#include "Notebook.h"
#include "FrontScene.h"
#include "TOCScene.h"
#include "EntryScene.h"
#include "EntryFile.h"
#include "Assert.h"

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
 
