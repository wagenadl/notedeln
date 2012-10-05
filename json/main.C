// main.C

#include "PageEditor.H"
#include <QApplication>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  PageEditor editor;
  editor.create("000.json");
  editor.show();
  return app.exec();
}
