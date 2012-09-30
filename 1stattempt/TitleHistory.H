// TitleHistory.H

#ifndef TITLEHISTORY_H

#define TITLEHISTORY_H

#include <QString>
#include <QDateTime>
#include <QDomElement>

class TitleHistory: public QObject {
  Q_OBJECT;
public:
  TitleHistory(QDomElement elt, QObject *parent=0);
  virtual ~TitleHistory();
  QString current() const;
  QString original() const;
  QStringList allVersions() const;
  QList<QDateTime> versionTimes() const;
  void newTitle(QString title);
signals:
  void changed();
private:
  QDomElement xml;
  QList<QDateTime> dates;
  QStringList titles;
};

#endif
