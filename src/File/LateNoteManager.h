// LateNoteManager.h

#ifndef LATENOTEMANAGER_H

#define LATENOTEMANAGER_H

#include "EntryData.h"
#include "LateNoteData.h"
#include "LateNoteFile.h"
#include <QDir>
#include <QSet>

class LateNoteManager: public QObject {
public:
  LateNoteManager(QString root, QObject *parent=0);
  virtual ~LateNoteManager() {}
  LateNoteData *newNote(QPointF sp0, QPointF sp1=QPointF());
  QSet<LateNoteData *> const &notes();
  void setBook(class Notebook *);
private:
  void ensureLoaded();
private:
  QDir dir;
  QSet<LateNoteData *> notes_;
  class Notebook *nb;
};

#endif
