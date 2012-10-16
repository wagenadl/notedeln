// PageView.H

#ifndef PAGEVIEW_H

#define PAGEVIEW_H

#include <QGraphicsView>

class PageView: public QGraphicsView {
  Q_OBJECT;
public:
  PageView(QWidget *parent=0);
  virtual ~PageView();
protected:
  void resizeEvent(QResizeEvent *);
};

#endif
