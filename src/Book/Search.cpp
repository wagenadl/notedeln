// Search.cpp

#include "Search.H"

#include "TextBlockData.H"
#include "TitleData.H"
#include "TableBlockData.H"
#include "LateNoteData.H"
#include "FootnoteData.H"
#include "Index.H"
#include "WordIndex.H"
#include "Assert.H"

#include <QSet>
#include <QDebug>
#include "TableData.H"

Search::Search(Notebook *book): book(book) {
}

Search::~Search() {
}

QString Search::untable(TableData const *tbld) {
  QString res = "";
  for (unsigned int r=0; r<tbld->rows(); r++) {
    if (r>0)
      res += "\n";
    for (unsigned int c=0; c<tbld->columns(); c++) {
      if (c>0)
        res += " | ";
      res += tbld->cellContents(r, c);
    }
  }
  return res;
}

void Search::addToResults(QList<SearchResult> &dest, QString phrase,
                          QString entryTitle,
                          Data const *data, int entryPage, int dataPage) {
  foreach (Data const *d, data->allChildren()) {
    TextData const *td = dynamic_cast<TextData const *>(d);
    if (td && td->text().toLower().contains(phrase)) {
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
      TableData const *tbld = dynamic_cast<TableData const *>(td);
      if (tbld)
        res.context = untable(tbld);
      else
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
    GfxNoteData const *nd = dynamic_cast<GfxNoteData const *>(d);
    int childSheet = nd ? nd->sheet() : -1;
    int childPage = childSheet>=0 ? entryPage + childSheet : dataPage;
    addToResults(dest, phrase, entryTitle, d, entryPage, childPage);
  }
}

QList<SearchResult> Search::immediatelyFindPhrase(QString phrase) const {
  QStringList words = phrase.toLower().split(QRegExp("\\s+"));
  QSet<int> entries = book->index()->words()->findWords(words, true);
  QList<int> sortedEntries = entries.toList();
  qSort(sortedEntries);

  QList<SearchResult> results;
  
  foreach (int pgno, sortedEntries) {
    CachedEntry ef(book->entry(pgno));
    ASSERT(ef);
    QString ttl = ef->titleText();
    foreach (TitleData const *bd, ef->children<TitleData>())
      addToResults(results, phrase, ttl, bd, pgno, pgno);
    foreach (BlockData const *bd, ef->children<BlockData>())
      addToResults(results, phrase, ttl, bd, pgno, pgno + bd->sheet());
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
    CachedEntry ef(book->entry(pgno));
    QString ttl = ef->titleText();
    foreach (TitleData const *bd, ef->children<TitleData>()) {
      mutex.lock();
      addToResults(results, phrase, ttl, bd, pgno, pgno);
      mutex.unlock();
    }
    foreach (BlockData const *bd, ef->children<BlockData>()) {
      mutex.lock();
      addToResults(results, phrase, ttl, bd, pgno, pgno + bd->sheet());
      mutex.unlock();
    }
  }
  if (!abandon)
    emit searchCompleted();
}
