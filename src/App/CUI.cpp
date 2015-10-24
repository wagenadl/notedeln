// CUI.cpp

#include "CUI.h"
#include "UUID.h"
#include <QSettings>
#include <QDateTime>

CUI::CUI() {
  QSettings s("net.danielwagenaar", "eln");

  if (s.contains("system/cui"))
    c = s.value("system/cui").toString();
  if (s.contains("system/cui1"))
    c1 = s.value("system/cui1").toString();
  if (s.contains("system/cui1"))
    c2 = s.value("system/cui1").toString();
  
  bool replace = true;
  if (s.contains("system/cui_t")) {
    QDateTime t0 = s.value("system/cui_t").toDateTime();
    replace = t0.secsTo(QDateTime::currentDateTime()) > 60*60*12;
  }

  if (replace) {
    c2 = c1;
    c1 = c;
    c = UUID::create(64);
    s.setValue("system/cui", c);
    s.setValue("system/cui1", c1);
    s.setValue("system/cui2", c2);
    s.setValue("system/cui_t", QDateTime::currentDateTime());
  }
}

bool CUI::match(QString s) const {
  return s==c || s==c1 || s==c2;
}

QString CUI::current() const {
  return c;
}
