// PopLabel.h

#ifndef POPLABEL_H

#define POPLABEL_H

#include <QLabel>

class PopLabel: public QLabel {
  Q_OBJECT;
public:
  PopLabel();
  virtual ~PopLabel() {}
  void leaveEvent(QEvent *);
  void enterEvent(QEvent *);
  void mousePressEvent(QMouseEvent *);
  void closeSoon();
signals:
  void clicked(Qt::KeyboardModifiers);
  void left();
protected:
  virtual void timerEvent(QTimerEvent*);
private:
  int timerID;
};

#endif
