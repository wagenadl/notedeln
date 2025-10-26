// App/CUI.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// CUI.cpp

#include "CUI.h"
#include "UUID.h"
#include <QSettings>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

CUI *CUI::instance() {
    static CUI *cui = 0;
    if (!cui) {
        cui = new CUI();
    }
    return cui;
}

CUI::CUI() {
  QSettings s("net.danielwagenaar", "eln");

  if (s.contains("system/cui"))
    c = s.value("system/cui").toString();
  if (s.contains("system/cui1"))
    c1 = s.value("system/cui1").toString();
  if (s.contains("system/cui2"))
    c2 = s.value("system/cui2").toString();
  
  bool replace = true;
  if (s.contains("system/cui_t")) {
    QDateTime t0 = s.value("system/cui_t").toDateTime();
    replace = t0.secsTo(QDateTime::currentDateTime()) > 60*60*12;
  }

  if (replace) {
    qDebug() << "Replacing cui at " << QDateTime::currentDateTime();
    if (s.contains("system/cui_t"))
        qDebug() << "last stamp " << s.value("system/cui_t").toDateTime();
    c2 = c1;
    c1 = c;
    c = UUID::create(64);
    s.setValue("system/cui", c);
    s.setValue("system/cui1", c1);
    s.setValue("system/cui2", c2);
    s.setValue("system/cui_t", QDateTime::currentDateTime());
  }
  
  // QDateTime th = QFileInfo(QDir::home().absolutePath()).created();
  QDateTime tr = QFileInfo(QDir::root().absolutePath()).birthTime();
  //  qint64 sh = th.toMSecsSinceEpoch()/1000;
  // Turns out, in ubuntu/cinnamon, the ~ directory gets changed upon login.
  qint64 sr = tr.toMSecsSinceEpoch()/1000;
  c = adjustID(c, sr);
  c1 = adjustID(c1, sr);
  c2 = adjustID(c2, sr);
}

bool CUI::match(QString s) const {
  return s==c || s==c1 || s==c2 || nocui();
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

bool &CUI::nocui() {
  static bool noc = false;
  return noc;
}

void CUI::globallyDisable() {
  nocui() = true;
}
