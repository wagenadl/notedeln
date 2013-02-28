// PastePosition.H

#ifndef PASTEPOSITION_H

#define PASTEPOSITION_H

#include <QPointF>

class PastePosition {
public:
  PastePosition(class QGraphicsScene *); // for paste by keyboard
  PastePosition(class QGraphicsScene *, QPointF scenePos); // for drag-and-drop
  bool inTextBlock();
  bool inText();
  QTextCursor cursor();
  QPointF scenePos();
private:
  bool intextblock;
  QTextCursor cursor_;
  QPointF scenepos;
};

#endif
