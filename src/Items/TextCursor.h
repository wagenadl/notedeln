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
  TextCursor(TextItemDoc *doc=0, int pos=0, int anc=-1);
  bool isValid() const;
  bool atStart() const;
  bool atEnd() const;
  void deleteChar();
  void deletePreviousChar();
  TextItemDoc *document();
  void insertText(QString);
  bool movePosition(MoveOperation op, MoveMode m=MoveAnchor);
  void setPosition(int pos, MoveMode m=MoveAnchor);
  QString selectedText() const;
  Range selectedRange() const;
  int selectionStart() const;
  int selectionEnd() const;
  bool hasSelection() const;
  void clearSelection();
private:
  void clampPosition();
private:
  TextItemDoc *doc;
  int pos;
  int anc;
};

#endif
