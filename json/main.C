// main.C

#include "PageEditor.H"
#include <QApplication>
#include <QFile>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  PageEditor editor;

  QFile f("000.json");
  if (f.exists())
    editor.load("000.json");
  else
    editor.create("000.json");
  
  editor.show();
  return app.exec();
}
