// WebPageMagician.C

#include "WebPageMagician.H"
#include "Assert.H"
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
