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
