// App/SearchView.h - This file is part of eln

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

// SearchView.H

#ifndef SEARCHVIEW_H

#define SEARCHVIEW_H

#include <QGraphicsView>

class SearchView: public QGraphicsView {
public:
  SearchView(class SearchResultScene *scene, QWidget *parent=0);
  /* We become the owner of the scene! */
  virtual ~SearchView();
protected:
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
  void wheelEvent(QWheelEvent *);
private:
  void gotoSheet(int n);
private:
  SearchResultScene *srs;
  double wheelDeltaAccum;
  double wheelDeltaStepSize;
  int currentSheet;
};

#endif
