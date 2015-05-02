// TableCell.cpp

#include "TableCell.h"
#include "TableData.h"
#include "Assert.h"
#include <QDebug>

bool TableCell::isValid() const {
  if (tbl && c>=0 && r>=0
      && c<tbl->columns()
      && r<tbl->rows())
    return true;
  qDebug() << "TableCell::notValid" << tbl << r << c
           << "(" << tbl->rows() << tbl->columns() << ")";
  return false;
}

bool TableCell::operator==(TableCell const &a) const {
  return tbl==a.tbl && c==a.c && r==a.r;
}

int TableCell::firstPosition() const {
  ASSERT(isValid());
  return tbl->cellStart(r, c);
}

int TableCell::lastPosition() const {
  ASSERT(isValid());
  return tbl->cellStart(r, c) + tbl->cellLength(r, c);
}

bool TableCell::isEmpty() const {
  ASSERT(isValid());
  return tbl->cellLength(r,c)==0;
}

int TableCell::length() const {
  ASSERT(isValid());
  return tbl->cellLength(r,c);
}
