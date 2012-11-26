// LateNoteItem.H

#ifndef LATENOTEITEM_H

#define LATENOTEITEM_H

#include "GfxNoteItem.H"

class LateNoteItem: public GfxNoteItem {
public:
  LateNoteItem(class LateNoteData *data, Item *parent=0);
  virtual ~LateNoteItem();
  static LateNoteItem *newNote(QPointF p0, QPointF p1, Item *parent);
};

#endif
