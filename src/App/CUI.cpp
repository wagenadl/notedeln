// CUI.cpp

#include "CUI.h"
#include "UUID.h"
#include <QSettings>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

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

  QDateTime th = QFileInfo(QDir::home().absolutePath()).created();
  QDateTime tr = QFileInfo(QDir::root().absolutePath()).created();
  qint64 sh = th.toMSecsSinceEpoch()/1000;
  qint64 sr = tr.toMSecsSinceEpoch()/1000;
  c = adjustID(c, sh, sr);
  c1 = adjustID(c1, sh, sr);
  c2 = adjustID(c2, sh, sr);
}

bool CUI::match(QString s) const {
  return s==c || s==c1 || s==c2;
}

QString CUI::current() const {
  return c;
}

QString CUI::adjustID(QString c, qint64 a, qint64 b) {
  return adjustID(adjustID(c, a), b);
}

QString CUI::adjustID(QString c, qint64 a) {
  int N = c.length();
  int k = N>16 ? 16 : N;
  qint64 ci = c.right(k).toULongLong(0, 16);
  ci ^= a;
  ci *= 17;
  ci += 5;
  QString c1 = QString("%1").arg(ci, 16, 16, QChar('0'));
  return c.left(N-k) + c1.right(k);
}
