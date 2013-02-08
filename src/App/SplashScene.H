// SplashScene.H

#ifndef SPLASHSCENE_H

#define SPLASHSCENE_H

#include <QGraphicsScene>
#include <QStringList>
#include <QPointer>

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
  void setWidget(QWidget *); // mark our view, lazy method
private:
  bool newRequested;
  bool openRequested;
  QString named;
  QPointer<QWidget> widget;
};

#endif
