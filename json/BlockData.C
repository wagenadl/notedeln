// BlockData.C

#include "BlockData.H"

#include "PageData.H"

BlockData::BlockData(class PageData *parent): Data(parent) {
}

BlockData::~BlockData() {
  // QObject will delete the notes
}

QList<class NoteData *> const &BlockData::notes() const {
  return notes_;
}

void BlockData::addNote(NoteData *n) {
  notes_.append(n);
  n->setParent(this);
  markModified();
}

void BlockData::loadMore(
