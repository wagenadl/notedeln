// ToolView.H

#ifndef TOOLVIEW_H

#define TOOLVIEW_H

#include <QGraphicsView>

class ToolView: public QGraphicsView {
  Q_OBJECT;
public:
  ToolView(class Mode *mode, QWidget *parent=0);
  virtual ~ToolView();
  class Toolbars *toolbars();
public slots:
  void setScale(double);
  void autoMask();
protected:
  //void resizeEvent(QResizeEvent *);
  void mousePressEvent(QMouseEvent *);
private:
  class ToolScene *tools;
};

#endif
