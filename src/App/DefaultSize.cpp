// App/DefaultSize.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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
