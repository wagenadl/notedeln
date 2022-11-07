// App/AlreadyOpen.cpp - This file is part of NotedELN

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

#include "AlreadyOpen.h"
#include "PageEditor.h"

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

AlreadyOpen::AlreadyOpen(QString name, PageEditor *w) {
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
  connect(server, &QLocalServer::newConnection, this, &AlreadyOpen::raise);
  connect(w, &PageEditor::newEditorCreated, this, &AlreadyOpen::addEditor);
  connect(w, &QWidget::destroyed, this, &AlreadyOpen::dropEditor);
}

AlreadyOpen::~AlreadyOpen() {
}

void AlreadyOpen::addEditor(PageEditor *w) {
  toBeRaised << w;
  connect(w, &PageEditor::newEditorCreated, this, &AlreadyOpen::addEditor);
  connect(w, &QObject::destroyed, this, &AlreadyOpen::dropEditor);
}

void AlreadyOpen::dropEditor(QObject *o) {
  QWidget *w = dynamic_cast<QWidget *>(o);
  toBeRaised.removeAll(w);
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
