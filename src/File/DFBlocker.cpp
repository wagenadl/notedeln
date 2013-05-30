// DFBlocker.cpp

#include "DFBlocker.H"
#include "DataFile.H"
#include <QTimerEvent>

DFBlocker::DFBlocker(QObject *parent): QObject(parent) {
  startTimer(DataFile0::maxBlockDuration()*1000);
}

DFBlocker::~DFBlocker() {
}

void DFBlocker::timerEvent(QTimerEvent *e) {
  killTimer(e->timerId());
  DataFile0::removeBlocker(this);
}
