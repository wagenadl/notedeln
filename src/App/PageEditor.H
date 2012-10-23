// PageEditor.H

#ifndef PAGEEDITOR_H

#define PAGEEDITOR_H

#include <QMainWindow>
#include "PageFile.H"

class PageEditor: public QMainWindow {
public:
  PageEditor();
  virtual ~PageEditor();
public slots:
  //  void load(QString fn);
  void open(PageFile *f);
  //  void create(QString fn);
protected:
  void closeEvent(QCloseEvent *);
private:
  class PageView *view_;
  class PageScene *scene_;
  PageFile *file_;
  class Toolbar *toolbar_;
};

#endif
