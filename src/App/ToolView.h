// ToolView.H

#ifndef TOOLVIEW_H

#define TOOLVIEW_H

#include <QGraphicsView>
#include <QDropEvent>

class ToolView: public QGraphicsView {
  Q_OBJECT;
public:
  ToolView(class Mode *mode, QWidget *parent=0);
  virtual ~ToolView();
  class Toolbars *toolbars();
signals:
  void drop(QDropEvent);
public slots:
  void setScale(double);
  void autoMask();
protected:
  //void resizeEvent(QResizeEvent *);
  void mousePressEvent(QMouseEvent *);
  void dragEnterEvent(QDragEnterEvent *);
  void dragMoveEvent(QDragMoveEvent *);
  void dropEvent(QDropEvent *);
private:
  class ToolScene *tools;
};

#endif
