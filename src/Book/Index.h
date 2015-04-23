// Index.H

#ifndef INDEX_H

#define INDEX_H

#include <QObject>
#include <QMap>

class Index: public QObject {
  Q_OBJECT;
public:
  Index(QString rootDir, class TOC *toc, QObject *parent);
  virtual ~Index();
  void watchEntry(class EntryData *);
  void unwatchEntry(class EntryData *);
  void deleteEntry(class EntryData *);
  void flush();
  class WordIndex *words() const;
private:
  bool flush(int pgno);
private:
  class WordIndex *widx;
  QMap<int, class WordSet *> pgs;
  QString rootdir;
};

#endif
