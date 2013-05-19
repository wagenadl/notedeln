// BlockData.C

#include "BlockData.H"
#include "PageData.H"
#include "NoteData.H"

BlockData::BlockData(Data *parent): Data(parent) {
  y0_ = 0;
  h_ = 0;
  sheet_ = -1;
  setType("block");
}

BlockData::~BlockData() {
  // QObject will delete the notes
}

double BlockData::y0() const {
  return y0_;
}

double BlockData::height() const {
  return h_;
}

int BlockData::sheet() const {
  return sheet_;
}

void BlockData::setY0(double y0) {
  y0_ = y0;
  markModified(InternalMod);
}

void BlockData::setHeight(double h) {
  h_ = h;
  markModified(InternalMod);
}

void BlockData::sneakilySetY0(double y0) {
  y0_ = y0;
}

void BlockData::sneakilySetHeight(double h) {
  h_ = h;
}

void BlockData::setSheet(int sheet) {
  sheet_ = sheet;
  markModified(InternalMod);
  if (!loading())
    emit newSheet(sheet);
}

bool BlockData::isEmpty() const {
  return true;
}
