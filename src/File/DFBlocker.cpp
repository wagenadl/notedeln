// File/DFBlocker.cpp - This file is part of NotedELN

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

// DFBlocker.cpp

#include "DFBlocker.h"
#include "DataFile.h"
#include <QTimerEvent>

DFBlocker::DFBlocker(QObject *parent): QObject(parent) {
  startTimer(DataFile0::maxBlockDuration()*1000);
}

DFBlocker::~DFBlocker() {
}

void DFBlocker::timerEvent(QTimerEvent *e) {
  killTimer(e->timerId());
  DataFile0::removeBlocker(this);
}
