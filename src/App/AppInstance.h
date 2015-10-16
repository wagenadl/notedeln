// AppInstance.h

#ifndef APPINSTANCE_H

#define APPINSTANCE_H

#include <QSet>
#include <QObject>

class AppInstance: public QObject {
  Q_OBJECT;
public:
  AppInstance(class App *app, class Notebook *nb);
  virtual ~AppInstance();
public slots:
  void forgetEditor(QObject *);
  void registerEditor(QObject *);
private:
  class Notebook *book;
  class SceneBank *bank;
  QSet<class PageEditor *> editors;
  class AlreadyOpen *aopen;
};

#endif
