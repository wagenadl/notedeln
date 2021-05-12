// App/ToolScene.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// ToolScene.cpp

#include "ToolScene.h"
#include "Toolbars.h"
#include "ClockFace.h"

ToolScene::ToolScene(Mode *mode, QObject *parent): QGraphicsScene(parent) {
  bars = new Toolbars(mode, 0);
  addItem(bars);
  bars->setPos(0, 0);
  toolbarrect = itemsBoundingRect();
  adjustedrect = toolbarrect.adjusted(-4, -39, 300, 2);

  clock = 0;
  setSceneRect(adjustedrect);
  setBackgroundBrush(QBrush(Qt::NoBrush));
}

ToolScene::~ToolScene() {
  // bars is deleted automatically because it has us as a parent
}

void ToolScene::showClock(bool yes) {
  if (yes) {
    if (!clock) {
      clock = new ClockFace;
      addItem(clock);
    }
  } else {
    delete clock;
    clock = 0;
  }
}

void ToolScene::moveClock(QRectF viewrect) {
  if (!clock)
    return;
  clock->setPos(viewrect.right() - 220, 0);
  clock->setScale(200);
  
  setSceneRect(adjustedrect | clock->mapRectToScene(clock->boundingRect()));
}
