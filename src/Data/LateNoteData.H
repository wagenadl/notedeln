// LateNoteData.H

#ifndef LATENOTEDATA_H

#define LATENOTEDATA_H

#include "GfxNoteData.H"

class LateNoteData: public GfxNoteData {
  Q_OBJECT;
public:
  LateNoteData(Data *parent=0);
  virtual ~LateNoteData();
  virtual void markModified(ModType mt=UserVisibleMod);
  /* This version of markModified never propagates, even for UserVisibleMod. */
};

#endif
