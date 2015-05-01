// webgrab/MozillaPage.H - This file is part of eln

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

// MozillaPage.H

#ifndef MOZILLAPAGE_H

#define MOZILLAPAGE_H

#include <QWebPage>

class MozillaPage: public QWebPage {
public:
  MozillaPage(QObject *parent=0);
  virtual ~MozillaPage();
protected:
  virtual QString userAgentForUrl(const QUrl &url) const;
};

#endif
