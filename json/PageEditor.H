// PageEditor.H

#ifndef PAGEEDITOR_H

#define PAGEEDITOR_H

#include <QMainWindow>

class PageEditor: public QMainWindow {
public:
  PageEditor();
  virtual ~PageEditor();
public slots:
  void load(QString fn);
  void create(QString fn);
private:
  class PageView *view_;
  class PageScene *scene_;
  class PageFile *file_;
  class Toolbar *toolbar_;
};

#endif
