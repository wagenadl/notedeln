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

  QPointF sp = parent->mapToScene(p1);
  if (p1.x()<p0.x()) 
    // item text will stick to the left
    d->setTextWidth(sp.x() - 5);
  else
    // item text will stick to the right
    d->setTextWidth(parent->style().real("page-width") - sp.x() - 5);
  
  LateNoteItem *i = new LateNoteItem(d, parent);
  i->makeWritable();
  i->setFocus();
  return i;
}
