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

#include "BookData.H"

static Data::Creator<BookData> c("book");

BookData::BookData(Data *parent): Data(parent) {
  title_ = "New book";
  author_ = "Me";
  address_ = "Here";
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

QDate BookData::startDate() const {
  return startDate_;
}

QDate BookData::endDate() const {
  return endDate_;
}

void BookData::setTitle(QString t) {
  title_ = t;
  markModified();
}

void BookData::setAuthor(QString t) {
  author_ = t;
  markModified();
}

void BookData::setAddress(QString t) {
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
