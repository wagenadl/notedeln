// Data.H

#ifndef DATA_H

#define DATA_H

#include <QObject>
#include <QDateTime>
#include <QString>

class Data: public QObject {
  Q_OBJECT;
  Q_PROPERTY(QDateTime created READ created WRITE setCreated)
  Q_PROPERTY(QDateTime modified READ modified WRITE setModified)
  Q_PROPERTY(QString type READ type WRITE setType)
  Q_PROPERTY(QString id READ id WRITE setId)
public:
  // constructor and destructor
  Data(Data *parent=0);
  virtual ~Data();
  // read properties
  QDateTime const &created() const;
  QDateTime const &modified() const;
  QString const &type() const;
  QString const &id() const;
  // write properties - not for casual use
  void setCreated(QDateTime const &);
  void setModified(QDateTime const &);
  void setType(QString const &);
  void setId(QString const &);
  // other
  void markModified();
  void load(QVariantMap const &);
  QVariantMap save() const;
signals:
  void modify();
protected:
  virtual void loadMore(QVariantMap const &);
  virtual void saveMore(QVariantMap &) const;
private:
  void loadProps(QVariantMap const &);
  void saveProps(QVariantMap &);
protected:
  QDateTime created_;
  QDateTime modified_;
  QString type_;
  QString id_;
private:
  bool loading_;
};

#endif
