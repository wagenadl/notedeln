// BookData.C

#include "BookData.H"

static Data::Creator<BookData> c("book");

BookData::BookData(Data *parent): Data(parent) {
  title_ = "New book";
  startDate_ = QDate::currentDate();
  endDate_ = QDate::currentDate();
  setType("book");
}

BookData::~BookData() {
}

QString BookData::title() const {
  return title_;
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
