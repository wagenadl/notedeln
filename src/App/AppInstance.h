// AppInstance.h

#ifndef APPINSTANCE_H

#define APPINSTANCE_H

#include <QSet>
#include <QObject>
#include <QDateTime>

class AppInstance: public QObject {
  Q_OBJECT;
public:
  AppInstance(class App *app, class Notebook *nb);
  virtual ~AppInstance();
private slots:
  void forgetEditor(QObject *);
  void registerEditor(QObject *);
  void commitSoonish();
  void commitNow();
  void updateNowUnless();
  void updateNow();
  void commitNowUnless();
  void committed(bool ok);
private:
  void setupVC();
private:
  class Notebook *book;
  class SceneBank *bank;
  QSet<class PageEditor *> editors;
  class AlreadyOpen *aopen;
  class QTimer *updateTimer;
  class QTimer *commitTimer;
  class BackgroundVC *backgroundVC;
  QDateTime mostRecentChange;
};

#endif
