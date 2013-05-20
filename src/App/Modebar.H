// App/Modebar.H - This file is part of eln

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

// Modebar.H

#ifndef MODEBAR_H

#define MODEBAR_H

#include "Toolbar.H"
#include "Mode.H"

class Modebar: public Toolbar {
  Q_OBJECT;
public:
  Modebar(Mode *mode, QGraphicsItem *parent);
  virtual ~Modebar();
public slots:
  void updateMode();
protected:
  virtual void doLeftClick(QString id);
private:
  static Mode::M idToMode(QString);
  static QString modeToId(Mode::M);
private:
  Mode *mode;
  
};

#endif
