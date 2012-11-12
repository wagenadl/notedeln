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
  Q_PROPERTY(QString frontImage READ frontImage WRITE setFrontImage)
  Q_PROPERTY(QDate startDate READ startDate WRITE setStartDate)
  Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate)
  ;
public:
  BookData(Data *parent=0);
  virtual ~BookData();
  QString title() const;
  QString author() const;
  QString address() const;
  QString frontImage() const;
  QDate startDate() const;
  QDate endDate() const;
  void setTitle(QString);
  void setAuthor(QString);
  void setAddress(QString);
  void setFrontImage(QString);
  void setStartDate(QDate);
  void setEndDate(QDate);
private:
  QString title_;
  QString author_;
  QString address_;
  QString frontImage_;
  QDate startDate_;
  QDate endDate_;
};

#endif
