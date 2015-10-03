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

#include "AlreadyOpen.h"

#include <QLocalServer>
#include <QLocalSocket>

#include <QMessageBox>
#include <QDebug>
#include <QDir>

static QString servername(QString fn) {
  QDir d(QDir::current());
  fn = d.absoluteFilePath(fn);
  fn = QDir::cleanPath(fn);
  return QString("eln-%1").arg(qHash(fn));
}

AlreadyOpen::AlreadyOpen(QString name, QWidget *w): QObject(w) {
  toBeRaised << w;
  server = new QLocalServer(this);
  QString sn = servername(name);
  if (!server->listen(sn)) {
    // old server lingering
    QLocalServer::removeServer(sn);
    if (!server->listen(sn)) {
      // couldn't remove. this is bad.
      qDebug() << "AlreadyOpen: Could not construct server. Sorry.";
    }
  }
  connect(server, SIGNAL(newConnection()), SLOT(raise()));
  connect(w, SIGNAL(newEditorCreated(QWidget *)), SLOT(addEditor(QWidget *)));
  connect(w, SIGNAL(destroyed(QObject *)), SLOT(dropEditor(QObject *)));
}

AlreadyOpen::~AlreadyOpen() {
  qDebug() << "~AlreadyOpen";
}

void AlreadyOpen::addEditor(QWidget *w) {
  qDebug() << "addEditor" << w;
  toBeRaised << w;
  connect(w, SIGNAL(newEditorCreated(QWidget *)), SLOT(addEditor(QWidget *)));
  connect(w, SIGNAL(destroyed(QObject *)), SLOT(dropEditor(QObject *)));
}

void AlreadyOpen::dropEditor(QObject *o) {
  QWidget *w = dynamic_cast<QWidget *>(o);
  qDebug() << "dropEditor" << w;
  bool cont = true;
  while (cont) {
    cont = false;
    for (auto it=toBeRaised.begin(); it!=toBeRaised.end(); ++it) {
      if (*it==w) {
	toBeRaised.erase(it);
	cont = true;
	break;
      }
    }
  }

  if (toBeRaised.isEmpty())
    deleteLater();
}

void AlreadyOpen::raise() {
  delete server->nextPendingConnection(); // open and close immediately
  bool ok = false;
  foreach (QWidget *w, toBeRaised) {
    if (w) {
      w->raise();
      ok = true;
    }
  }
  if (ok)
    return;
  
  qDebug() << "AlreadyOpen: All windows disappeared. Cannot raise";
}

bool AlreadyOpen::check(QString fn) {
  QLocalSocket s;
  s.connectToServer(servername(fn));
  bool r = s.waitForConnected(100);
  return r;
}
