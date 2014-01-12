
// Search.H

#ifndef SEARCH_H

#define SEARCH_H

#include <QString>
#include <QDateTime>
#include <QList>
#include <QThread>
#include <QMutex>

#include "Notebook.H"

struct SearchResult {
  enum Type {
    Unknown,
    InTextBlock,
    InTableBlock,
    InGfxNote,
    InLateNote,
    InFootnote,
  };
  Type type;
  int page;
  int startPageOfEntry;
  QString phrase; // search text
  QString context; // entire text of containing object
  QString entryTitle;
  QDateTime cre, mod; // of containing object
  QList<int> whereInContext;
};

class Search: public QThread {
  Q_OBJECT;
public:
  Search(Notebook *book);
  virtual ~Search();
  QList<SearchResult> immediatelyFindPhrase(QString) const;
  void startSearchForPhrase(QString);
  void abandonSearch();
  bool isSearchComplete();
  bool isSearching();
  QList<SearchResult> searchResults() const;
signals:
  void searchCompleted();
private:
  static void addToResults(QList<SearchResult> &dest, QString phrase,
                           QString entryTitle,
                           Data const *data, int entryPage, int dataPage);
  void run();
private:
  Notebook *book;
  QString phrase;
  QList<SearchResult> results;
  bool abandon;
  mutable QMutex mutex;
};

#endif
