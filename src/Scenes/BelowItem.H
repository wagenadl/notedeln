// BelowItem.H

#ifndef BELOWITEM_H

#define BELOWITEM_H

#include <QGraphicsRectItem>

class BelowItem: public QGraphicsRectItem {
public:
  BelowItem(QGraphicsItem *parent=0);
protected:
  virtual void dropEvent(class QGraphicsSceneDragDropEvent *e);
};

#endif
