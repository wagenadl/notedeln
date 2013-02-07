// SplashView.H

#ifndef SPLASHVIEW_H

#define SPLASHVIEW_H

#include <QGraphicsView>

class SplashView: public QGraphicsView {
  Q_OBJECT;
public:
  SplashView();
  virtual ~SplashView();
protected:
  void closeEvent(class QCloseEvent *);
  void resizeEvent(class QResizeEvent *);
signals:
  void closing();
};

#endif
