// LateNoteItem.C

#include "LateNoteItem.H"
#include "LateNoteData.H"
#include "TextItem.H"

#include <QBrush>


static Item::Creator<LateNoteData, LateNoteItem> c("latenote");

LateNoteItem::LateNoteItem(LateNoteData *data, Item *parent):
  GfxNoteItem(data, parent) {
  if (line)
    line->setPen(QPen(QBrush(QColor(style().string("latenote-line-color"))),
		      style().real("latenote-line-width")));
  text->setDefaultTextColor(QColor(style().string("latenote-text-color")));
  if (data->isRecent())
    makeWritable();
}

LateNoteItem::~LateNoteItem() {
}

LateNoteItem *LateNoteItem::newNote(QPointF p0, QPointF p1, Item *parent) {
  Q_ASSERT(parent);
  LateNoteData *d = new LateNoteData(parent->data());
  d->setPos(p0);
  d->setEndPoint(p1);
  
  LateNoteItem *i = new LateNoteItem(d, parent);
  i->makeWritable();
  i->setFocus();
  return i;
}
