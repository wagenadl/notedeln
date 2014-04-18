// App/PageEditor.H - This file is part of eln

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

// PageEditor.H

#ifndef PAGEEDITOR_H

#define PAGEEDITOR_H

#include <QMainWindow>

class PageEditor: public QMainWindow {
  Q_OBJECT;
public:
  PageEditor(class SceneBank *bank);
  virtual ~PageEditor();
public slots:
  void gotoEntryPage(QString s);
  void gotoTOC(int n=1); // n>=1
  void gotoFront();
protected:
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
private:
  class SceneBank *bank;
  class PageView *view;
  class ToolView *toolview;
};

#endif
