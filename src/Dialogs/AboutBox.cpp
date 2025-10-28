// Dialogs/AboutBox.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// AboutBox.cpp

#include "AboutBox.h"
#include "Translate.h"
#include "Version.h"
#include <QMessageBox>
#include <QDebug>

void AboutBox::about() {
  QString author = "Daniel A. Wagenaar";
  QString summary = Translate::_("eln-is");
  QString manuref = Translate::_("where-man");
  QString gpl = Translate::_("gpl");
  QString eln = Translate::_("eln");
  QString ttl = "<b>" + eln + "</b> " + Version::toString();
  QString copyright = QString::fromUtf8("(C) 2013–")
    + QString::number(Version::buildDate().year())
    + " " + author;
  QString msg = ttl + "<br>"
    + copyright + "<br><br>"
    + summary + " "
    + manuref + "<br><br>"
    + gpl;

  QMessageBox box;
  box.setWindowTitle(Translate::_("about-eln"));
  box.setTextFormat(Qt::RichText);
  box.setText(msg);
  box.setIcon(QMessageBox::Information);
  int targetwidth = box.width() / 8;
  int iconwidth = targetwidth;
  //while (iconwidth < targetwidth)
  //  iconwidth *= 2;
  QPixmap pm = QPixmap(":/eln512.png")
    .scaled(QSize(2*iconwidth,2*iconwidth),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
  pm.setDevicePixelRatio(2);
  box.setIconPixmap(pm);
  box.exec();
}
