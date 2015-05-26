// DefaultSize.cpp

#include "DefaultSize.h"
#include <QDesktopWidget>
#include <QApplication>
#include <math.h>

namespace DefaultSize {
  QSize onScreenSize(QSizeF booksize) {
    QDesktopWidget *d = QApplication::desktop();
    QWidget *s = d->screen();
    double dpi = sqrt(s->physicalDpiX() * 1.0 * s->physicalDpiY());
    double bookw = booksize.width() * dpi/72;
    double bookh = booksize.height() * dpi/72;
    double bookrat = bookh/bookw;

    double deskw = d->availableGeometry().width() - 10;
    double deskh = d->availableGeometry().height() - 30;
    // Subtract some space for window decorations
    double deskrat = deskh/deskw;

    double usew = deskw;
    double useh = deskh;
    if (deskrat<bookrat)
      // desktop height is limiting factor
      usew = useh / bookrat;
    else
      useh = usew * bookrat;
    if (bookh<useh*.75) 
      return (QSizeF(usew, useh) * .75).toSize();
    else if (bookh<useh) 
      return QSize(bookw, bookh);
    else
      return QSize(usew, useh);
  }      
};
