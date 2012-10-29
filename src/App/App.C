// App.C

#include "App.H"
#include "ModSnooper.H"
#include <stdio.h>

App::App(int &argc, char **argv): QApplication(argc, argv) {
  ms = new ModSnooper();
}

App::~App() {
  delete ms;
}

bool App::x11EventFilter(XEvent *event) {
  ms->x11Event(event);
  return QApplication::x11EventFilter(event);
}

App *App::instance() {
  return dynamic_cast<App*>(QApplication::instance());
}

ModSnooper *App::modSnooper() {
  return ms;
}
