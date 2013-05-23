// BelowItem.cpp

#include "BelowItem.H"
#include "EntryScene.H"
#include <QGraphicsSceneDragDropEvent>
#include <QDebug>
#include <QMimeData>
#include <QUrl>

BelowItem::BelowItem(QGraphicsItem *parent): QGraphicsRectItem(parent) {
  setAcceptDrops(true);
}

void BelowItem::dropEvent(QGraphicsSceneDragDropEvent *e) {
  EntryScene *s = dynamic_cast<EntryScene*>(scene());


  qDebug() << "drop below"
	   << e->mimeData()->hasImage()
	   << e->mimeData()->hasUrls()
	   << e->mimeData()->hasText();
  foreach (QUrl u, e->mimeData()->urls())
    qDebug() << "   url" << u.toString();
  if (e->mimeData()->hasText())
    qDebug() << "   text" << e->mimeData()->text();

  
  if (s && s->dropBelow(e->scenePos(), e->mimeData()))
    e->setDropAction(Qt::CopyAction);
  else
    e->setDropAction(Qt::IgnoreAction);
}
