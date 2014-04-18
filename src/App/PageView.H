// App/PageView.H - This file is part of eln

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

// PageView.H

#ifndef PAGEVIEW_H

#define PAGEVIEW_H

#include <QGraphicsView>
#include <QMap>

class PageView: public QGraphicsView {
  Q_OBJECT;
public:
  PageView(class Notebook *nb, QWidget *parent=0);
  virtual ~PageView();
public slots:
  void gotoEntryPage(int pgno, int dir=0);
  void gotoTOC(int pgno=1);
  void gotoFront();
  void nextPage();
  void goRelative(int n);
  void previousPage();
  void lastPage();
  void openFindDialog();
  void htmlDialog();
signals:
  void onEntryPage(int, int); // start-page-of-entry, sheet-no
  void onFrontMatter(int);
  void scaled(double);
public:
  class Mode *mode() const;
  class Notebook *notebook() const;
protected:
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
  void keyReleaseEvent(QKeyEvent *);
  void mousePressEvent(QMouseEvent *);
  void wheelEvent(QWheelEvent *);
private:
  void leavePage();
  void printDialog();
  void createContinuationEntry();
  void focusEntry();
private slots:
  bool gotoSheet(int n);
  void notebookReloaded(QMap<int,int>);
private:
  class Notebook *book; // we do not own!
  class FrontScene *frontScene;
  class TOCScene *tocScene;
  class EntryScene *entryScene;
  int currentPage;
  int currentSheet;
  enum {
    Front,
    TOC,
    Entries,
  } currentSection;
  class DeletedStack *deletedStack;
  int wheelDeltaAccum;
  int wheelDeltaStepSize;
  class SearchDialog *searchDialog;
};

#endif
