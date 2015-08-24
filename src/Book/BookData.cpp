// Book/BookData.cpp - This file is part of eln

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

// BookData.C

#include "BookData.h"
#include "Notebook.h"
#include "Translate.h"
#include "UserInfo.h"

static Data::Creator<BookData> c("book");

BookData::BookData(Data *parent): Data(parent) {
  title_ = "";
  QString user = UserInfo::fullName();
  author_ = user.isEmpty() ? "" : user;
  address_ = "";
  startDate_ = QDate::currentDate();
  endDate_ = QDate::currentDate();
  setType("book");

  nb = 0;
}

BookData::~BookData() {
}

QString BookData::title() const {
  return title_;
}

QString BookData::author() const {
  return author_;
}

QString BookData::address() const {
  return address_;
}

QString BookData::otitle() const {
  return otitle_==title_ ? "" : otitle_;
}

QString BookData::oauthor() const {
  return oauthor_==author_ ? "" : oauthor_;
}

QString BookData::oaddress() const {
  return oaddress_==address_ ? "" : oaddress_;
}

QDate BookData::startDate() const {
  return startDate_;
}

QDate BookData::endDate() const {
  return endDate_;
}

void BookData::setTitle(QString t) {
  if (title_==t)
    return;
  if (!isRecent() && otitle_.isEmpty() && !(book() && book()->isReadOnly()))
    otitle_ = title_;
  title_ = t;
  markModified();
}

void BookData::setAuthor(QString t) {
  if (author_==t)
    return;
  if (!isRecent() && oauthor_.isEmpty() && !(book() && book()->isReadOnly()))
    oauthor_ = author_;
  author_ = t;
  markModified();
}

void BookData::setAddress(QString t) {
  if (address_==t)
    return;
  if (!isRecent() && oaddress_.isEmpty() && !(book() && book()->isReadOnly()))
    oaddress_ = address_;
  address_ = t;
  markModified();
}

void BookData::setStartDate(QDate d) {
  startDate_ = d;
  markModified();
}

void BookData::setEndDate(QDate d) {
  bool chg = d!=endDate_;
  endDate_ = d;
  if (chg)
    markModified();
}

void BookData::setBook(Notebook *n) {
  nb = n;
}

Notebook *BookData::book() const {
  return nb;
}

void BookData::loadMore(QVariantMap const &src) {
  Data::loadMore(src);
  oauthor_ = src.contains("oauthor") ? src["oauthor"].toString() : "";
  otitle_ = src.contains("otitle") ? src["otitle"].toString() : "";
  oaddress_ = src.contains("oaddress") ? src["oaddress"].toString() : "";
}

void BookData::saveMore(QVariantMap  &dst) const {
  Data::saveMore(dst);
  if (!oauthor_.isEmpty())
    dst["oauthor"] = oauthor_;
  if (!otitle_.isEmpty())
    dst["otitle"] = otitle_;
  if (!oaddress_.isEmpty())
    dst["oaddress"] = oaddress_;

}

