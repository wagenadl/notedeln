// LateNoteItem.H

#ifndef LATENOTEITEM_H

#define LATENOTEITEM_H

#include "GfxNoteItem.H"
#include "LateNoteData.H"

class LateNoteItem: public GfxNoteItem {
public:
  LateNoteItem(LateNoteData *data, Item *parent=0);
  virtual ~LateNoteItem();
  DATAACCESS(LateNoteData);
  static LateNoteItem *newNote(QPointF p0, QPointF p1, Item *parent);
  virtual void setScale(qreal f);
  virtual bool excludeFromNet() const;
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private:
  void prepDateItem();
  void setDateItemPosition();
private:
  QGraphicsTextItem *dateItem;
  
};

#endif
