// GfxNoteItem.H

#ifndef GFXNOTEITEM_H

#define GFXNOTEITEM_H

#include "Item.H"
#include <QGraphicsObject>

class GfxNoteItem: public QGraphicsObject, public Item {
  Q_OBJECT;  
public:
  GfxNoteItem(class GfxNoteData *data, Item *parent=0);
  virtual ~GfxNoteItem();
  GfxNoteData *data();
  QRectF boundingRect() const;
  QRectF netBoundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  static GfxNoteItem *newNote(QPointF p0, Item *parent);
  static GfxNoteItem *newNote(QPointF p0, QPointF p1, Item *parent);
  virtual void makeWritable();
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private slots:
  void updateTextPos();
  void childMousePress(QPointF, Qt::MouseButton);
  void abandon();
private:
  GfxNoteData *data_;
  class TextItem *text;
  QGraphicsLineItem *line;
};

#endif
