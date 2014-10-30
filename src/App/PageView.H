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
#include "CachedPointer.H"
#include <QDropEvent>

class PageView: public QGraphicsView {
  Q_OBJECT;
public:
  PageView(class SceneBank *bank, QWidget *parent=0);
  virtual ~PageView();
public:
  enum Section {
    Front,
    TOC,
    Entries,
  };
  Section section() const;
  QString pageName() const;
  int pageNumber() const;
public slots:
  void pageNumberClick(int, Qt::KeyboardModifiers);
  PageView *newView(QString); // opens new view on a named entry page
  void gotoEntryPage(QString);
  void gotoEntryPage(int pgno, int dir=0);
  void gotoTOC(int pgno=1);
  void gotoFront();
  void nextPage();
  void goRelative(int n);
  void previousPage();
  void lastPage();
  void openFindDialog();
  void htmlDialog();
  void drop(QDropEvent);
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
  //  void dragEnterEvent(QDragEnterEvent *);
  //  void dragMoveEvent(QDragMoveEvent *);
private:
  void leavePage();
  void printDialog();
  void createContinuationEntry();
  void focusEntry();
  void markEventView();
private slots:
  bool gotoSheet(int n);
  void notebookReloaded(QMap<int,int>);
  void modeChange();
private:
  class SceneBank *bank; // we do not own!
  class Notebook *book; // we do not own!
  CachedPointer<class EntryScene> entryScene;
  int currentPage;
  int currentSheet;
  Section currentSection;
  class DeletedStack *deletedStack;
  int wheelDeltaAccum;
  int wheelDeltaStepSize;
  class SearchDialog *searchDialog;
};

#endif
