// Book/BookData.H - This file is part of eln

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

// BookData.H

#ifndef BOOKDATA_H

#define BOOKDATA_H

#include "Data.H"
#include <QDate>

class BookData: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString title READ title WRITE setTitle)
  Q_PROPERTY(QString author READ author WRITE setAuthor)
  Q_PROPERTY(QString address READ address WRITE setAddress)
  Q_PROPERTY(QDate startDate READ startDate WRITE setStartDate)
  Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate)
  ;
public:
  BookData(Data *parent=0);
  virtual ~BookData();
  QString title() const;
  QString author() const;
  QString address() const;
  QDate startDate() const;
  QDate endDate() const;
  void setTitle(QString);
  void setAuthor(QString);
  void setAddress(QString);
  void setStartDate(QDate);
  void setEndDate(QDate);
  void setBook(class Notebook *nb);
  virtual Notebook *book() const;
private:
  QString title_;
  QString author_;
  QString address_;
  QDate startDate_;
  QDate endDate_;
  Notebook *nb;
};

#endif
