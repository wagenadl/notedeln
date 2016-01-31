// PopLabel.cpp

#include "PopLabel.h"
#include <QDebug>
#include <QMouseEvent>

PopLabel::PopLabel(): QLabel(0, Qt::FramelessWindowHint) {
  timerID = 0;
}

void PopLabel::leaveEvent(QEvent *) {
  emit left();
}

void PopLabel::closeSoon() {
  if (!timerID)
    timerID = startTimer(500);
  qDebug() << "timerid: " << timerID;
}

void PopLabel::timerEvent(QTimerEvent*) {
  emit left();
}

void PopLabel::enterEvent(QEvent *) {
  if (timerID) {
    killTimer(timerID);
    timerID = 0;
  }
}

void PopLabel::mousePressEvent(QMouseEvent *e) {
  emit clicked(e->modifiers());
}
