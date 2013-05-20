// ResourceMagic/WebPageMagician.H - This file is part of eln

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

// WebPageMagician.H

#ifndef WEBPAGEMAGICIAN_H

#define WEBPAGEMAGICIAN_H

#include "Magician.H"
#include <QVariant>

class WebPageLinkMagician: public SimpleMagician {
public:
  WebPageLinkMagician(QVariantMap const &dict);
  /* dict must contain keys:
       "re": a regexp
       "web": a url with %1
       "link-key": an attribute name for an <a> element
       "link-value": an attribute value for that <a> element
  */
  bool objectUrlNeedsWebPage(QString) const;
  QUrl objectUrlFromWebPage(QString, QString) const;
private:
  QString key;
  QString val;  
};

#endif
