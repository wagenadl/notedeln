// PageEditor.H

#ifndef PAGEEDITOR_H

#define PAGEEDITOR_H

#include <QMainWindow>

class PageEditor: public QMainWindow {
  Q_OBJECT;
public:
  PageEditor(class Notebook *nb);
  virtual ~PageEditor();
private:
  class Notebook *book;
  class PageView *view;
};

#endif
