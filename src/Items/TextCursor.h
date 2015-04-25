// TextCursor.h

#ifndef TEXTCURSOR_H

#define TEXTCURSOR_H

#include "TextItemDoc.h"

class TextCursor {
public:
  enum MoveMode {
    MoveAnchor,
    KeepAnchor,
  };
  enum Location {
    Start,
    End,
  }
public:
  TextCursor(TextItemDoc *doc, int pos=0);
  bool atStart() const;
  bool atEnd() const;
  void deleteChar();
  void deletePreviousChar();
  TextItemDoc *document();
  void insertText(QString);
  QString selectedText() const;
};

#endif
