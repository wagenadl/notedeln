// LateNoteData.C

#include "LateNoteData.H"

static Data::Creator<LateNoteData> c("latenote");

LateNoteData::LateNoteData(Data *parent): GfxNoteData(parent) {
  setType("latenote");
}

LateNoteData::~LateNoteData() {
}

void LateNoteData::markModified(ModType mt) {
  if (mt==UserVisibleMod)
    mt = NonPropMod;
  GfxNoteData::markModified(mt);
}

