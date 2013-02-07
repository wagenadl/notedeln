// SplashScene.H

#ifndef SPLASHSCENE_H

#define SPLASHSCENE_H

#include <QGraphicsScene>
#include <QStringList>

class SplashScene: public QGraphicsScene {
  Q_OBJECT;
public:
  SplashScene(QObject *parent=0);
  virtual ~SplashScene();
  static QStringList localNotebooks();
  static class Notebook *openNotebook();
private slots:
  void createNew();
  void openExisting();
  void openNamed(QString);
signals:
  void done();
private:
  void makeBackground();
  void makeItems();
private:
  bool newRequested;
  bool openRequested;
  QString named;
};

#endif
