// Cursors.H

#ifndef CURSORS_H

#define CURSORS_H

#include <QCursor>

class Cursors {
public:
  static QCursor &refine(QCursor &);
  static QCursor refined(Qt::CursorShape cs);
  static QCursor crossCursor();
  static QPixmap &crossPixmap();
};

#endif
