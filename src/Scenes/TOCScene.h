// Scenes/TOCScene.H - This file is part of NotedELN

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

// TOCScene.H

#ifndef TOCSCENE_H

#define TOCSCENE_H

#include "BaseScene.h"
#include <QMap>

class TOCScene: public BaseScene {
  Q_OBJECT;
public:
  TOCScene(class TOC *data, QObject *parent=0);
  virtual void populate();
  virtual ~TOCScene();
  virtual QString title() const;
  virtual QString pgNoToString(int) const;
  int sheetForPage(int) const;
public slots:
  void tocChanged();
  void itemChanged();
  //  virtual void gotoSheet(int);
private slots:
  void pageNumberClicked(int, Qt::KeyboardModifiers);
private:
  void rebuild();
  void relayout();
private:
  TOC *data;
  QList<class TOCItem *> items;
  QList<class QGraphicsLineItem *> lines;
  QMap<int, int> page2sheet;
};

#endif
