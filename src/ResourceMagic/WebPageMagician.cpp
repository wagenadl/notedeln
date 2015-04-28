// ResourceMagic/WebPageMagician.cpp - This file is part of eln

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

// WebPageMagician.C

#include "WebPageMagician.h"
#include "Assert.h"
#include <QDebug>
#include <QWebPage>
#include <QWebElement>
#include <QWebFrame>

WebPageLinkMagician::WebPageLinkMagician(QVariantMap const &dict):
  SimpleMagician(dict) {
  /* dict must contain keys:
       "re": a regexp
       "web": a url with %1
       "link-key": an attribute name for an <a> element
       "link-value": an attribute value for that <a> element
  */
  key = dict["link-key"].toString();
  val = dict["link-value"].toString();  
}

bool WebPageLinkMagician::objectUrlNeedsWebPage(QString) const {
  return true;
}

QUrl WebPageLinkMagician::objectUrlFromWebPage(QString tag,
					       QString html) const {
  QWebPage pg;
  QWebFrame *frm = pg.mainFrame();
  QWebElement doc = frm->documentElement();
  doc.appendInside(html);
  QString query = QString("a[%1=\"%2\"]").arg(key).arg(val);
  qDebug() << "Querying " << query;
  QWebElement elt = doc.findFirst(query);
  qDebug() << "elt = " << !elt.isNull() << elt.attribute("href");
  if (elt.isNull())
    return webUrl(tag); // cop out
  else
    return QUrl(elt.attribute("href"));
}
