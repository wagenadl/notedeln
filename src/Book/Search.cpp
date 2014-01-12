// Search.cpp

#include "Search.H"

#include "TextBlockData.H"
#include "TableBlockData.H"
#include "LateNoteData.H"
#include "FootnoteData.H"
#include "Index.H"
#include "WordIndex.H"
#include "Assert.H"

#include <QSet>
#include <QDebug>

Search::Search(Notebook *book): book(book) {
}

Search::~Search() {
}

void Search::addToResults(QList<SearchResult> &dest, QString phrase,
                          QString entryTitle,
                          Data const *data, int entryPage, int dataPage) {
  foreach (Data const *d, data->allChildren()) {
    TextData const *td = dynamic_cast<TextData const *>(d);
    if (td && td->text().contains(phrase)) {
      // gotcha
      SearchResult res;
      res.phrase = phrase;
      if (dynamic_cast<TableBlockData const *>(data))
        res.type = SearchResult::InTableBlock;
      else if (dynamic_cast<TextBlockData const *>(data))
        res.type = SearchResult::InTextBlock;
      else if (dynamic_cast<TextBlockData const *>(data))
        res.type = SearchResult::InTextBlock;
      else if (dynamic_cast<LateNoteData const *>(data))
        res.type = SearchResult::InLateNote;
      else if (dynamic_cast<GfxNoteData const *>(data))
        res.type = SearchResult::InGfxNote;
      else if (dynamic_cast<FootnoteData const *>(data))
        res.type = SearchResult::InFootnote;
      else
        res.type = SearchResult::Unknown;
      res.page = dataPage;
      res.startPageOfEntry = entryPage;
      res.entryTitle = entryTitle;
      res.context = td->text();
      res.cre = td->created();
      res.mod = td->modified();
      int i0 = res.context.indexOf(phrase, 0, Qt::CaseInsensitive);
      while (i0>=0) {
        res.whereInContext << i0;
        i0 = res.context.indexOf(phrase, i0+1, Qt::CaseInsensitive);
      }
      dest << res;      
    }
    addToResults(dest, phrase, entryTitle, d, entryPage, dataPage);
  }
}

QList<SearchResult> Search::immediatelyFindPhrase(QString phrase) const {
  QStringList words = phrase.toLower().split(QRegExp("\\s+"));
  QSet<int> entries = book->index()->words()->findWords(words, true);

  QList<SearchResult> results;
  
  foreach (int pgno, entries) {
    EntryFile *ef = book->pageIfCached(pgno);
    bool preloaded = ef;
    if (!preloaded)
      ef = ::loadPage(book->filePath("pages"), pgno, 0);
    ASSERT(ef);
    QString ttl = ef->data()->titleText();
    foreach (BlockData const *bd, ef->data()->children<BlockData>())
      addToResults(results, phrase, ttl, bd, pgno, pgno + bd->sheet());
    if (!preloaded)
      delete ef;
  }

  return results;
}

void Search::startSearchForPhrase(QString s) {
  if (isRunning()) {
    abandon = true;
    if (!wait(2000)) {
      qDebug() << "Search: could not stop thread nicely.";
      terminate();
    }
  }

  phrase = s;
  results.clear();
  abandon = false;
  start();
}

void Search::abandonSearch() {
  abandon = true;
}

bool Search::isSearchComplete() {
  return !isRunning() && !abandon;
}

bool Search::isSearching() {
  return isRunning();
}

QList<SearchResult> Search::searchResults() const {
  mutex.lock();
  QList<SearchResult> res = results;
  mutex.unlock();
  return res;
}

      
void Search::run() {
  QStringList words = phrase.toLower().split(QRegExp("\\s+"));
  QSet<int> entries = book->index()->words()->findWords(words, true);

  foreach (int pgno, entries) {
    if (abandon)
      return;
    EntryFile *ef = ::loadPage(book->filePath("pages"), pgno, this);
    ASSERT(ef);
    QString ttl = ef->data()->titleText();
    foreach (BlockData const *bd, ef->data()->children<BlockData>()) {
      mutex.lock();
      addToResults(results, phrase, ttl, bd, pgno, pgno + bd->sheet());
      mutex.unlock();
    }
    delete ef;
  }
  if (!abandon)
    emit searchCompleted();
}
