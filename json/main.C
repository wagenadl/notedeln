// main.C

#include "PageEditor.H"
#include <QApplication>
#include <QFile>
#include <QDir>
#include "Notebook.H"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  PageEditor editor;
  QDir d("test.nb");
  Notebook *nb = d.exists()
    ? Notebook::load("test.nb")
    : Notebook::create("test.nb");
  Q_ASSERT(nb);

  editor.open(nb->hasPage(1) ? nb->page(1) : nb->createPage(1));
  
  editor.show();
  return app.exec();
}
