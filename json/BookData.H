// BookData.H

#ifndef BOOKDATA_H

#define BOOKDATA_H

#include "Data.H"
#include <QDate>

class BookData: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString title READ title WRITE setTitle)
  Q_PROPERTY(QDate startDate READ startDate WRITE setStartDate)
  Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate)
  ;
public:
  BookData(Data *parent=0);
  virtual ~BookData();
  QString title() const;
  QDate startDate() const;
  QDate endDate() const;
  void setTitle(QString);
  void setStartDate(QDate);
  void setEndDate(QDate);
private:
  QString title_;
  QDate startDate_;
  QDate endDate_;
};

#endif
