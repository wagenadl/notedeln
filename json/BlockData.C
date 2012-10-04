// BlockData.C

#include "BlockData.H"
#include "PageData.H"
#include "NoteData.H"

BlockData::BlockData(class PageData *parent): Data(parent) {
}

BlockData::~BlockData() {
  // QObject will delete the notes
}

double BlockData::y0() const {
  return y0_;
}

double BlockData::h() const {
  return h_;
}

void BlockData::setY0(double y0) {
  y0_ = y0;
  markModified();
}

void BlockData::setH(double h) {
  h_ = h;
  markModified();
}

QList<class NoteData *> const &BlockData::notes() const {
  return notes_;
}

void BlockData::addNote(NoteData *n) {
  notes_.append(n);
  n->setParent(this);
  markModified();
}

void BlockData::loadMore(QVariantMap const &src) {
  foreach (NoteData *nd, notes_)
    delete nd;
  notes_.clear();

  QVariantList nl = src["notes"].toList();
  foreach (QVariant n, nl) {
    NoteData *nd = new NoteData(this);
    nd->load(n.toMap());
    notes_.append(nd);
  }
}

void BlockData::saveMore(QVariantMap &dst) const {
  QVariantList nl;
  foreach (NoteData *nd, notes_) {
    QVariantMap n = nd->save();
    nl.append(n);
  }
  dst["notes"] = nl;
}
