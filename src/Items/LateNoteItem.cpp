// LateNoteItem.C

#include "LateNoteItem.H"
#include "LateNoteData.H"
#include "TextItem.H"
#include "EntryData.H"
#include "Assert.H"

#include <QBrush>
#include <QDebug>

static Item::Creator<LateNoteData, LateNoteItem> c("latenote");

LateNoteItem::LateNoteItem(LateNoteData *data, Item *parent):
  GfxNoteItem(data, parent) {
  if (line)
    line->setPen(QPen(QBrush(QColor(style().string("latenote-line-color"))),
		      style().real("latenote-line-width")));
  text->setDefaultTextColor(QColor(style().string("latenote-text-color")));
  text->setFont(style().font("latenote-font"));
  prepDateItem();
  if (data->isRecent()) {
    makeWritable();
  }
}

LateNoteItem::~LateNoteItem() {
}

void LateNoteItem::prepDateItem() {
  dateItem = new QGraphicsTextItem(this);
  dateItem->setFont(style().font("latenote-font"));
  dateItem->setDefaultTextColor(QColor(style().string("latenote-text-color")));
  QDateTime myDate = data()->created();
  QDateTime pgDate = data()->page()->created();
  QString lbl;
  if (myDate.date()==pgDate.date())
    lbl = "";
  else if (myDate.date().year() == pgDate.date().year())
    lbl = myDate.toString(style().string("date-format-yearless"));
  else
    lbl = myDate.toString(style().string("date-format"));
  dateItem->setPlainText(lbl);
  setDateItemPosition();
}

void LateNoteItem::setDateItemPosition() {
  QPointF sp = scenePos();
  QPointF tp = text->pos();
  QRectF br = dateItem->sceneBoundingRect();
  double ml = style().real("margin-left");
  if (mapToScene(tp).x()>=ml) 
    dateItem->setPos(QPointF(ml - br.width() - 2 - sp.x(), tp.y()));
  else
    dateItem->setPos(tp.x(), tp.y() - style().real("latenote-yshift"));
}

LateNoteItem *LateNoteItem::newNote(QPointF p0, QPointF p1, Item *parent) {
  ASSERT(parent);
  LateNoteData *d = new LateNoteData(parent->data());
  QPointF sp0 = parent->mapToScene(p0);
  QPointF sp1 = parent->mapToScene(p1);
  d->setPos(p0);
  d->setDelta(sp1-sp0);
  if (p1.x()<p0.x()) 
    // item text will stick to the left
    d->setTextWidth(sp1.x() - 5);
  else
    // item text will stick to the right
    d->setTextWidth(parent->style().real("page-width") - sp1.x() - 5);
  
  LateNoteItem *i = new LateNoteItem(d, parent);
  i->makeWritable();
  i->setFocus();
  return i;
}

void LateNoteItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  GfxNoteItem::mouseMoveEvent(e);
  setDateItemPosition();
}

void LateNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  GfxNoteItem::mouseReleaseEvent(e);
  setDateItemPosition();
}

void LateNoteItem::setScale(qreal f) {
  GfxNoteItem::setScale(f);
  setDateItemPosition();
}

bool LateNoteItem::excludeFromNet() const {
  return true;
}
