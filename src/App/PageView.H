// PageView.H

#ifndef PAGEVIEW_H

#define PAGEVIEW_H

#include <QGraphicsView>

class PageView: public QGraphicsView {
  Q_OBJECT;
public:
  PageView(QWidget *parent=0);
  virtual ~PageView();
signals:
  void pgUp();
  void pgDn();
  void home();
  void end();
protected:
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
  void keyReleaseEvent(QKeyEvent *);
};

#endif
