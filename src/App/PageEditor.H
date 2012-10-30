// PageEditor.H

#ifndef PAGEEDITOR_H

#define PAGEEDITOR_H

#include <QMainWindow>

class PageEditor: public QMainWindow {
  Q_OBJECT;
public:
  PageEditor(class Notebook *nb);
  virtual ~PageEditor();
public slots:
  void gotoPage(int pgno);
  void gotoTOC(int pgno=1);
  void gotoFront();
  //  void gotoIndex();
  void nextPage();
  void previousPage();
  void lastPage();
private:
  class Notebook *book; // we do not own!
  class PageView *view;
  class FrontScene *frontScene;
  class TOCScene *tocScene;
  class PageScene *pageScene;
  // class Toolbar *toolbar;
  int currentPage;
  enum {
    Front,
    TOC,
    Pages
  } currentSection;
};

#endif
