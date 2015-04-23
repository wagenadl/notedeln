// Cursors.cpp

#include "Cursors.h"
#include <QPixmap>

QPixmap &Cursors::crossPixmap() {
  static QPixmap ch(":icons/crosshair.png");
  return ch;
}


QCursor Cursors::crossCursor() {
  return QCursor(crossPixmap(), 16, 16);
}

QCursor Cursors::refined(Qt::CursorShape cs) {
  if (cs==Qt::CrossCursor)
    return crossCursor();
  else
    return QCursor(cs);
}

