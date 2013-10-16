// App/AlreadyOpen.cpp - This file is part of eln

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

#include "AlreadyOpen.H"

#include <QLocalServer>
#include <QLocalSocket>

#include <QMessageBox>
#include <QDebug>

static QString servername(QString fn) {
  return QString("eln-%1").arg(qHash(fn));
}

AlreadyOpen::AlreadyOpen(QString name, QWidget *w): QObject(w), toBeRaised(w) {
  server = new QLocalServer(this);
  QString sn = servername(name);
  if (!server->listen(sn)) {
    // old server lingering
    QLocalServer::removeServer(name);
    if (!server->listen(sn)) {
      // couldn't remove. this is bad.
      qDebug() << "AlreadyOpen: Could not construct server. Oh well.";
    }
  }
  connect(server, SIGNAL(newConnection()),
	  SLOT(raise()));
}

AlreadyOpen::~AlreadyOpen() {
}

void AlreadyOpen::raise() {
  delete server->nextPendingConnection(); // open and close immediately
  QWidget *w = toBeRaised;
  if (w) {
    qDebug() << "AlreadyOpen: Raising";
    w->raise();
  } else {
    qDebug() << "AlreadyOpen: Window disappeared. Cannot raise";
  }
}

bool AlreadyOpen::check(QString fn) {
  QLocalSocket s;
  s.connectToServer(servername(fn));
  bool r = s.waitForConnected(100);
  // graphical confirmation would be nice too
  if (r)
    qDebug() << "AlreadyOpen: found a running instance";
  else
    qDebug() << "AlreadyOpen: no running instance found";
  return r;
}
