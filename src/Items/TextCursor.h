// Items/TextCursor.h - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    PreviousWord, NextWord,
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
  bool atStartOfWord() const;
  int deleteChar(); // return number of QChars deleted; this can be more ...
  int deletePreviousChar(); // ... than one if combining marks are involved
  TextItemDoc *document() const;
  void insertText(QString);
  bool movePosition(MoveOperation op, MoveMode m=MoveAnchor);
  void correctPosition(int n); // shifts by n raw characters w/o regard ...
  // ... for combining marks
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
  void skipForwardOverCombining();
  void skipBackwardOverCombining();
private:
  TextItemDoc *doc;
  int pos;
  mutable int anc;
};

#endif
