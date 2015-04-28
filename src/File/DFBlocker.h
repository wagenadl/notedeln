// DFBlocker.H

#ifndef DFBLOCKER_H

#define DFBLOCKER_H

#include <QObject>

class DFBlocker: public QObject {
  Q_OBJECT;
public:
  DFBlocker(QObject *parent);
  virtual ~DFBlocker();
  virtual void timerEvent(class QTimerEvent *);
};

#endif
