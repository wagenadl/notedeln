// App/ToolScene.cpp - This file is part of eln

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

// ToolScene.cpp

#include "ToolScene.h"
#include "Toolbars.h"

ToolScene::ToolScene(Mode *mode, QObject *parent): QGraphicsScene(parent) {
  bars = new Toolbars(mode, 0);
  addItem(bars);
  bars->setPos(0, 0);
  setSceneRect(itemsBoundingRect().adjusted(-4, -39, 2, 2));
  setBackgroundBrush(QBrush(Qt::NoBrush));
}

ToolScene::~ToolScene() {
  // bars is deleted automatically because it has us as a parent
}
