// MozillaPage.C

#include "MozillaPage.H"
#include <QDebug>

MozillaPage::MozillaPage(QObject *parent): QWebPage(parent) {
}

MozillaPage::~MozillaPage() {
}

QString MozillaPage::userAgentForUrl(const QUrl &url) const {
  qDebug() << "MozillaPage: " << url;
  return "Mozilla/5.0 (Windows NT 6.2; Win64; x64; rv:16.0.1) Gecko/20121011 Firefox/16.0.1";
}
