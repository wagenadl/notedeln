// App/AlreadyOpen.H - This file is part of NotedELN

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

#ifndef ALREADYOPEN_H

#define ALREADYOPEN_H

#include <QObject>
#include <QPointer>
#include <QList>

class AlreadyOpen: public QObject {
  Q_OBJECT;
public:
  static bool check(QString fn);
public:
  AlreadyOpen(QString fn, class PageEditor *parent);
  virtual ~AlreadyOpen();
private slots:
  void raise();
  void addEditor(PageEditor *);
  void dropEditor(QObject *);
private:
  QList<QPointer<QWidget> > toBeRaised;
  class QLocalServer *server;
};

#endif
