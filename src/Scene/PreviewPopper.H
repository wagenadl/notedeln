// PreviewPopper.H

#ifndef PREVIEWPOPPER_H

#define PREVIEWPOPPER_H

#include <QWidget>

class PreviewPopper: public QObject {
  Q_OBJECT;
public:
  PreviewPopper(class ResourceManager *resmgr, QString resname,
		QPoint center, QObject *parent);
  virtual ~PreviewPopper();
  QWidget *popup(); // open popup immediately and return widget pointer
  // or zero if resource not available
protected:
  virtual void timerEvent(QTimerEvent *);
private:
  void smartPosition();
private:
  ResourceManager *resmgr;
  QString resname;
  QPoint center;
  class QWidget *widget;
  int timerID;
};

#endif
