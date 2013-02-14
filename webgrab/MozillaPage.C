// MozillaPage.C

#include "MozillaPage.H"
#include <QDebug>

MozillaPage::MozillaPage(QObject *parent): QWebPage(parent) {
}

MozillaPage::~MozillaPage() {
}

QString MozillaPage::userAgentForUrl(const QUrl &url) const {
  qDebug() << "MozillaPage: " << url;
  return "Mozilla Firefox";
}
