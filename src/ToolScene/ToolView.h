// App/ToolView.h - This file is part of NotedELN

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

// ToolView.H

#ifndef TOOLVIEW_H

#define TOOLVIEW_H

#include <QGraphicsView>
#include <QDropEvent>

class ToolView: public QGraphicsView {
  Q_OBJECT;
public:
  ToolView(class Mode *mode, QWidget *parent=0);
  virtual ~ToolView();
  class Toolbars *toolbars();
signals:
  void drop(QDropEvent);
public slots:
  void setScale(double);
  void autoMask();
  void setFullScreen(bool);
  void hideSlowly();
  virtual void show();
private slots:
  void timeout();
protected:
  void resizeEvent(QResizeEvent *);
  void mousePressEvent(QMouseEvent *);
  void dragEnterEvent(QDragEnterEvent *);
  void dragMoveEvent(QDragMoveEvent *);
  void dropEvent(QDropEvent *);
private:
  class ToolScene *tools;
  class QTimer *timer;
  double opacity;
};

#endif
