// Book/TOCEntry.cpp - This file is part of eln

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

// TOCEntry.C

#include "TOCEntry.h"
#include <QDebug>

static Data::Creator<TOCEntry> c("entry");

TOCEntry::TOCEntry(Data *parent): Data(parent) {
  setType("entry");
  sheetCount_ = 1;
  startPage_ = 1;
}

TOCEntry::~TOCEntry() {
}

int TOCEntry::startPage() const {
  return startPage_;
}

QString TOCEntry::title() const {
  return title_;
}

int TOCEntry::sheetCount() const {
  return sheetCount_;
}

bool TOCEntry::contains(int pgno) const {
  return pgno >= startPage()
    && pgno < startPage() + sheetCount();
}

bool TOCEntry::contains(QString pgno) const {
  QRegExp re("(\\d\\d*)([a-z]?)");
  if (!re.exactMatch(pgno))
    return false;
  int n = re.cap(1).toInt();
  QString a = re.cap(2);
  if (a=="")
    return contains(n);
  else
    return contains(n) && contains(n + 1 + a[0].unicode() - 'a');
}

int TOCEntry::sheetOf(QString pgno) const {
  QRegExp re("(\\d\\d*)([a-z]?)");
  if (!re.exactMatch(pgno))
    return -1;
  int n = re.cap(1).toInt();
  QString a = re.cap(2);
  if (a=="")
    return n - startPage();
  else
    return n + 1 + a[0].unicode() - 'a' - startPage();
}

QDateTime TOCEntry::lastSeen() const {
  return seen_;
}

void TOCEntry::setStartPage(int n) {
  startPage_ = n;
  markModified();
}

void TOCEntry::setTitle(QString t) {
  title_ = t;
  markModified();
}

void TOCEntry::setSheetCount(int n) {
  sheetCount_ = n;
  markModified();
}

void TOCEntry::setLastSeen(QDateTime const &t) {
  seen_ = t;
  markModified();
}
