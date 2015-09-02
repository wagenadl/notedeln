// File/DFBlocker.h - This file is part of eln

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

// DFBlocker.H

#ifndef DFBLOCKER_H

#define DFBLOCKER_H

#include <QObject>

class DFBlocker: public QObject {
  Q_OBJECT;
public:
  DFBlocker(QObject *parent);
  virtual ~DFBlocker();
  virtual void timerEvent(class QTimerEvent *);
};

#endif
