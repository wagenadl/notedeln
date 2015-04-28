// App/SplashScene.H - This file is part of eln

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

// SplashScene.H

#ifndef SPLASHSCENE_H

#define SPLASHSCENE_H

#include <QGraphicsScene>
#include <QStringList>
#include <QPointer>

class SplashScene: public QGraphicsScene {
  Q_OBJECT;
public:
  SplashScene(QObject *parent=0);
  virtual ~SplashScene();
  static QStringList localNotebooks();
  static class Notebook *openNotebook();
private slots:
  void createNew();
  void openExisting();
  void openNamed(QString);
signals:
  void done();
private:
  void makeBackground();
  void makeItems();
  void setWidget(QWidget *); // mark our view, lazy method
private:
  bool newRequested;
  bool openRequested;
  QString named;
  QPointer<QWidget> widget;
};

#endif
