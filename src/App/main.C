// main.C

#include "PageEditor.H"
#include <QApplication>
#include <QFile>
#include <QDir>
#include "Notebook.H"
#include "App.H"

int main(int argc, char **argv) {
  App app(argc, argv);
  QDir d("test.nb");
  Notebook *nb = d.exists()
    ? Notebook::load("test.nb")
    : Notebook::create("test.nb");
  Q_ASSERT(nb);

  PageEditor *editor = new PageEditor(nb);
  editor->resize(editor->sizeHint());
  editor->show();
  int r = app.exec();
  delete editor;
  delete nb; // this flushes
  return r;
}
