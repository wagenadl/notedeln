// LateNoteManager.cpp

#include "LateNoteManager.h"
#include "Assert.h"
#include <QDebug>

LateNoteManager::LateNoteManager(QString root, QObject *parent):
  Data(0), dir(root) {
  QObject::setParent(parent);
  nb = 0;
  loaded = false;
}

QList<LateNoteData *> LateNoteManager::notes() {
  ensureLoaded();
  return children<LateNoteData>();
}

void LateNoteManager::ensureLoaded() {
  if (loaded)
    return;
  if (!dir.exists())
    return;
  QStringList flt; flt << "*.json";
  QStringList entries = dir.entryList(flt, QDir::Files);
  for (auto fn: entries) {
    LateNoteFile *f = loadLateNoteFile(dir, fn.left(fn.indexOf(".")), this);
    files << f;
    LateNoteData *d = f->data();
    d->setBook(nb);
    addChild(d);
  }
  loaded = true;
}

LateNoteData *LateNoteManager::newNote(QPointF p0, QPointF p1) {
  if (!dir.exists()) 
    QDir("/").mkpath(dir.absolutePath());
  LateNoteFile *f = createLateNoteFile(dir, this);
  files << f;
  LateNoteData *d = f->data();
  d->setBook(nb);
  d->setPos(p0);
  if (!p1.isNull())
    d->setDelta(p1-p0);
  d->setTextWidth(0);
  addChild(d);
  return d;
}
  

void LateNoteManager::setBook(Notebook *b) {
  nb = b;
}

void LateNoteManager::addChild(Data *d, ModType mt) {
  QString u = d->uuid();
  // This does _not_ restore if file deleted.
  Data::addChild(d, mt);
}

Data *LateNoteManager::takeChild(Data *d, ModType mt) {
  QString u = d->uuid();
  deleteLateNoteFile(dir, u);
  return Data::takeChild(d, mt);
}

bool LateNoteManager::saveAll() {
  bool ok = true;
  for (LateNoteFile *f: files) 
    if (f && f->needToSave())
      ok &= f->saveNow();
  return ok;
}
