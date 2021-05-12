// App/AppInstance.h - This file is part of NotedELN

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

// AppInstance.h

#ifndef APPINSTANCE_H

#define APPINSTANCE_H

#include <QSet>
#include <QObject>
#include <QDateTime>

class AppInstance: public QObject {
  Q_OBJECT;
public:
  AppInstance(class App *app, class Notebook *nb);
  virtual ~AppInstance();
private slots:
  void forgetEditor(QObject *);
  void registerEditor(QObject *);
  void commitSoonish();
  void commitNow();
  void updateNowUnless();
  void updateNow();
  void commitNowUnless();
  void committed(bool ok);
private:
  void setupVC();
private:
  class Notebook *book;
  class SceneBank *bank;
  QSet<class PageEditor *> editors;
  class AlreadyOpen *aopen;
  class QTimer *updateTimer;
  class QTimer *commitTimer;
  class BackgroundVC *backgroundVC;
  QDateTime mostRecentChange;
};

#endif
