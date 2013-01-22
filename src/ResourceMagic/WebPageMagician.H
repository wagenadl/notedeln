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
