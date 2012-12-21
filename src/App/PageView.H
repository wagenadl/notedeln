// PageView.H

#ifndef PAGEVIEW_H

#define PAGEVIEW_H

#include <QGraphicsView>

class PageView: public QGraphicsView {
  Q_OBJECT;
public:
  PageView(class Notebook *nb, QWidget *parent=0);
  virtual ~PageView();
  class Mode const *mode() const { return mode_; }
public slots:
  void gotoPage(int pgno);
  void gotoTOC(int pgno=1);
  void gotoFront();
  //  void gotoIndex();
  void nextPage();
  void previousPage();
  void lastPage();
protected:
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
  void keyReleaseEvent(QKeyEvent *);
  void mousePressEvent(QMouseEvent *);
private:
  void leavePage();
  void printDialog();
private slots:
  void nowOnPage(int pgno);
private:
  class Notebook *book; // we do not own!
  class FrontScene *frontScene;
  class TOCScene *tocScene;
  class PageScene *pageScene;
  class GfxPalette *linePalette;
  class GfxPalette *markPalette;
  // class Toolbar *toolbar;
  int currentPage;
  enum {
    Front,
    TOC,
    Pages
  } currentSection;
  class DeletedStack *deletedStack;
  class Toolbars *toolbars;
  class Mode *mode_;
};

#endif
