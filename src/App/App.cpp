// App.C

#include "App.H"
#include <stdio.h>

App::App(int &argc, char **argv): QApplication(argc, argv) {
}

App::~App() {
}

App *App::instance() {
  return dynamic_cast<App*>(QApplication::instance());
}
