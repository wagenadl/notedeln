// App.H

#ifndef APP_H

#define APP_H

#include <QApplication>

class App: public QApplication {
  Q_OBJECT;
public:
  App(int &argc, char **argv);
  virtual ~App();
public:
  class ModSnooper *modSnooper();
  static App *instance();
  virtual bool x11EventFilter(XEvent *event);
private:
  ModSnooper *ms;
};

#endif
