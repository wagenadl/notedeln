// LateNoteManager.h

#ifndef LATENOTEMANAGER_H

#define LATENOTEMANAGER_H

#include "EntryData.h"
#include "LateNoteData.h"
#include "LateNoteFile.h"
#include <QDir>

class LateNoteManager: public Data {
public:
  LateNoteManager(QString root, QObject *parent=0);
  virtual ~LateNoteManager() {}
  LateNoteData *newNote(QPointF sp0, QPointF sp1=QPointF());
  QList<LateNoteData *> notes();
  void setBook(class Notebook *);
  virtual void addChild(Data *, ModType mt=UserVisibleMod);
  virtual Data *takeChild(Data *, ModType mt=UserVisibleMod);
private:
  void ensureLoaded();
private:
  QDir dir;
  bool loaded;
  class Notebook *nb;
};

#endif
