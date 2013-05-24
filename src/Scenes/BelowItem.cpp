// Scenes/BelowItem.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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
