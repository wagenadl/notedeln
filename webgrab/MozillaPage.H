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
