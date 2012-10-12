// TOC.H

#ifndef TOC_H

#define TOC_H

#include <QObject>
#include <QVariant>

/* I should probably split TOC into TOCData and TOCFile so that the
   whole JSON business is separated from the core functionality.
*/

class TOC: public QObject {
  Q_OBJECT;
public:
  class Entry {
  public:
    int startPage;
    QString fn;
    QString title;
  public:
    Entry(int startPage=0, QString fn="", QString title="");
    bool operator<(Entry const &) const;
    QVariantMap save() const;
    void load(QVariantMap const &);
  };
public:
  TOC(QString fn, QObject *parent=0); // loads fn
  virtual ~TOC();
  QList<Entry> const &entries() const;
  void addEntry(int startPage, QString fn, QString title);
  void setStartPage(QString fn, int startPage);
  void setTitle(QString fn, QString title);
  bool save() const;
  static TOC *create(QString fn, QObject *parent=0)
private:
  TOC(QObject *parent=0); // doesn't load
private:
  QList<Entry> entries_;
  QString fn;
};

#endif
