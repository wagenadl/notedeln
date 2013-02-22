// GfxNoteItem.H

#ifndef GFXNOTEITEM_H

#define GFXNOTEITEM_H

#include "Item.H"
#include <QGraphicsObject>
#include "GfxNoteData.H"

class GfxNoteItem: public Item {
  Q_OBJECT;  
public:
  GfxNoteItem(GfxNoteData *data, Item *parent=0);
  virtual ~GfxNoteItem();
  DATAACCESS(GfxNoteData);
  QRectF boundingRect() const;
  QRectF netBoundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  static GfxNoteItem *newNote(QPointF p0, QPointF p1, Item *parent);
  virtual void makeWritable();
  void childMousePress(QPointF, Qt::MouseButton, bool resizeFlag);
  class TextItem *textItem() { return text; }
  virtual void setScale(qreal f);
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private slots:
  void updateTextPos(); // must be called after scale change
  void abandon();
protected:
  class TextItem *text;
  QGraphicsLineItem *line;
  bool resizing;
  double initialTextWidth;
};

#endif
