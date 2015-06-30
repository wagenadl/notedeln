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
  enum MoveOperation {
    NoMove,
    Left, Right,
    Start, End,
    Up, Down,
    StartOfLine, EndOfLine,
    NextCell, PreviousCell,
    StartOfWord, EndOfWord,
  };
  class Range {
  public:
    Range(int a=0, int b=0);
    int start() const { return s; }
    int end() const { return e; }
    int length() const { return e-s; }
    int size() const { return e-s; }
    bool isEmpty() const { return size()==0; }
  private:
    int s;
    int e;
  };
public:
  TextCursor(class TextItemDoc *doc=0, int pos=0, int anc=-1);
  bool isValid() const;
  bool isNull() const { return !isValid(); }
  bool atStart() const;
  bool atEnd() const;
  void deleteChar();
  void deletePreviousChar();
  TextItemDoc *document() const;
  void insertText(QString);
  bool movePosition(MoveOperation op, MoveMode m=MoveAnchor);
  void setPosition(int pos, MoveMode m=MoveAnchor);
  void exchangePositionAndAnchor();
  QString selectedText() const;
  Range selectedRange() const;
  int selectionStart() const;
  int selectionEnd() const;
  bool hasSelection() const;
  void clearSelection(MoveOperation reason=NoMove);
  void selectAround(int pos, MoveOperation startscope, MoveOperation endscope);
  int position() const;
  int anchor() const;
  bool operator==(TextCursor const &) const;
  bool operator!=(TextCursor const &a) const { return !operator==(a); }
  TextCursor findForward(QString) const; // start of string not before POS
  TextCursor findBackward(QString) const; // end of string not after POS
  TextCursor findForward(QRegExp) const;
  TextCursor findBackward(QRegExp) const;
  /* findForward and findBackward ignore any selection in the source cursor. */
  void clampPosition();
private:
  TextItemDoc *doc;
  int pos;
  int anc;
};

#endif
