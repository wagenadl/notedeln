// SearchResult.h

#ifndef SEARCHRESULT_H

#define SEARCHRESULT_H

#include <QList>
#include <QString>
#include <QDateTime>

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
  QString uuid; // of containing object
  QList<int> whereInContext;
};

#endif
