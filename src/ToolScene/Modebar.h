// App/Modebar.H - This file is part of NotedELN

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

// Modebar.H

#ifndef MODEBAR_H

#define MODEBAR_H

#include "Toolbar.h"
#include "Mode.h"

class Modebar: public Toolbar {
  Q_OBJECT;
public:
  Modebar(Mode *mode, QGraphicsItem *parent);
  virtual ~Modebar();
public slots:
  void updateMode();
  void updateTypeMode();
  void updateDrawMode();
protected slots:
  virtual void doubleClicked(Qt::KeyboardModifiers);
protected:
  virtual void doLeftClick(QString id, Qt::KeyboardModifiers);
private:
  static Mode::M idToMode(QString);
  static QString modeToId(Mode::M);
private:
  Mode *mode;
private:
  class ToolItem *typeModeItem;
  class ToolItem *moveModeItem;
  class MarkSizeItem *markModeItem;
  class LineWidthItem *sketchModeItem;
  
};

#endif
