// NoteData.C

#include "NoteData.H"
#include "BlockData.H"

NoteData::NoteData(class BlockData *parent): Data(parent) {
  setType("note");
}

NoteData::~NoteData() {
}

double NoteData::x() const {
  return x_;
}

double NoteData::y() const {
  return y_;
}

QString NoteData::text() const {
  return text_;
}

void NoteData::setX(double x) {
  x_ = x;
  markModified();
}


void NoteData::setY(double y) {
  y_ = y;
  markModified();
}

void NoteData::setText(QString const &t) {
  text_ = t;
  markModified();
}
