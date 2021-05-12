// Gui/PopLabel.cpp - This file is part of NotedELN

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

// PopLabel.cpp

#include "PopLabel.h"
#include <QDebug>
#include <QMouseEvent>

PopLabel::PopLabel(): QLabel(0, Qt::FramelessWindowHint) {
  timerID = 0;
}

void PopLabel::leaveEvent(QEvent *) {
  emit left();
}

void PopLabel::closeSoon() {
  if (!timerID)
    timerID = startTimer(500);
  qDebug() << "timerid: " << timerID;
}

void PopLabel::timerEvent(QTimerEvent*) {
  emit left();
}

void PopLabel::enterEvent(QEvent *) {
  if (timerID) {
    killTimer(timerID);
    timerID = 0;
  }
}

void PopLabel::mousePressEvent(QMouseEvent *e) {
  emit clicked(e->modifiers());
}
