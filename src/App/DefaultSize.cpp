// DefaultSize.cpp

#include "DefaultSize.h"
#include <QDesktopWidget>
#include <QApplication>


namespace DefaultSize {
  QSize onScreenSize(QSizeF booksize) {
    double bookw = booksize.width();
    double bookh = booksize.height();
    double bookrat = bookh/bookw;

    double deskw = QApplication::desktop()->width();
    double deskh = QApplication::desktop()->height();
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
