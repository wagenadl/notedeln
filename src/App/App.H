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
  static App *instance();
};

#endif
