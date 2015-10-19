// LateNoteManager.cpp

#include "LateNoteManager.h"
#include "Assert.h"
#include <QDebug>

LateNoteManager::LateNoteManager(QString root, QObject *parent):
  Data(0), dir(root) {
  QObject::setParent(parent);
  qDebug() << "LateNoteManager" << root << this;
  nb = 0;
}

QSet<LateNoteData *> const &LateNoteManager::notes() {
  ensureLoaded();
  return notes_;
}

void LateNoteManager::ensureLoaded() {
  if (!notes_.isEmpty())
    return;
  if (!dir.exists())
    return;
  QStringList flt; flt << "*.json";
  QStringList entries = dir.entryList(flt, QDir::Files);
  for (auto fn: entries) {
    LateNoteFile *f = loadLateNoteFile(dir, fn.left(fn.indexOf(".")), this);
    LateNoteData *d = f->data();
    d->setBook(nb);
    addChild(d);
    notes_ << d;
  }
}

LateNoteData *LateNoteManager::newNote(QPointF p0, QPointF p1) {
  qDebug() << "LateNoteManager::newNote" << dir.absolutePath() << dir.exists();
  if (!dir.exists()) 
    QDir("/").mkpath(dir.absolutePath());
  LateNoteFile *f = createLateNoteFile(dir, this);
  LateNoteData *d = f->data();
  d->setBook(nb);
  d->setPos(p0);
  if (!p1.isNull())
    d->setDelta(p1-p0);
  d->setTextWidth(0);
  notes_ << d;
  addChild(d);
  return d;
}
  

void LateNoteManager::setBook(Notebook *b) {
  nb = b;
}

void LateNoteManager::addChild(Data *d, ModType mt) {
  QString u = d->uuid();
  qDebug() << "LNM: add" << u;
  // This does _not_ restore if file deleted.
  Data::addChild(d, mt);
}

Data *LateNoteManager::takeChild(Data *d, ModType mt) {
  QString u = d->uuid();
  deleteLateNoteFile(dir, u);
  return Data::takeChild(d, mt);
}
