// LateNoteManager.cpp

#include "LateNoteManager.h"

#include <QDebug>

LateNoteManager::LateNoteManager(QString root, QObject *parent):
  QObject(parent), dir(root) {
  qDebug() << "LateNoteManager" << root;
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
  d->setParent(this);
  return d;
}
  

void LateNoteManager::setBook(Notebook *b) {
  nb = b;
}
