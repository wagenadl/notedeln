// Items/TextCursor.cpp - This file is part of NotedELN

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

// TextCursor.cpp

#include "TextCursor.h"
#include <QDebug>
#include "ElnAssert.h"
#include "Unicode.h"

TextCursor::Range::Range(int a, int b) {
  if (a<b) {
    s = a;
    e = b;
  } else {
    s = b;
    e = a;
  }
}  

TextCursor::TextCursor(TextItemDoc *doc, int pos, int anc):
  doc(doc), pos(pos), anc(anc) {
  if (doc && pos<doc->firstPosition())
    pos = doc->firstPosition();
}
                                                   
bool TextCursor::isValid() const {
  if (!doc)
    return false;
  return pos>=doc->firstPosition() && pos<=doc->lastPosition()
    && (anc<0 || (anc>=doc->firstPosition() && anc<=doc->lastPosition()));
}

bool TextCursor::atStart() const {
  return doc ? pos<=doc->firstPosition() : false;
}

bool TextCursor::atStartOfWord() const {
  if (atStart())
    return true;
  return doc ? doc->characterAt(pos-1).isSpace() : false;
}

bool TextCursor::atEnd() const {
  return doc ? pos>=doc->lastPosition() : false;
}

void TextCursor::clearSelection(TextCursor::MoveOperation) {
  anc = -1;
}

int TextCursor::deleteChar() {
  ASSERT(doc);
  if (hasSelection()) {
    Range r = selectedRange();
    QPair<int, int> rm = doc->removeWithCombining(r.start(), r.size());
    pos = rm.first;
    clearSelection();
    return rm.second;
  } else {
    QPair<int, int> rm = doc->removeWithCombining(pos, 1);
    pos = rm.first;
    return rm.second;
  }
}

void TextCursor::correctPosition(int n) {
  pos += n;
  if (pos<doc->firstPosition())
    pos = doc->firstPosition();
  else if (pos>doc->lastPosition())
    pos = doc->lastPosition();
}

int TextCursor::deletePreviousChar() {
  ASSERT(doc);
  if (hasSelection()) {
    Range r = selectedRange();
    QPair<int, int> rm = doc->removeWithCombining(r.start(), r.size());
    pos = rm.first;
    clearSelection();
    return rm.second;
  } else {
    QPair<int, int> rm = doc->removeWithCombining(pos - 1, 1);
    pos = rm.first;
    return rm.second;
  }
}

TextItemDoc *TextCursor::document() const {
  return doc;
}

void TextCursor::insertText(QString s) {
  ASSERT(doc);
  if (hasSelection())
    deleteChar();
  s.replace("\t", "    ");

  // move cursor forward to respect diacriticals
  QString t0 = doc->text();
  while (pos<doc->lastPosition() && Unicode::isCombining(t0[pos]))
    pos++;
  doc->insert(pos, s);
  pos += s.size();
}

QString TextCursor::selectedText() const {
  ASSERT(doc);
  if (!hasSelection())
    return "";
  Range r = selectedRange();
  return doc->selectedText(r.start(), r.end());
}

bool TextCursor::movePosition(TextCursor::MoveOperation op,
                              TextCursor::MoveMode m) {
  int p0 = pos;
  
  ASSERT(doc);
  
  if (m==KeepAnchor) {
    if (!hasSelection())
      anc = pos;
  } else {
    clearSelection(op);
  }

  switch (op) {
  case NoMove:
    break;
  case PreviousWord:
      if (atStartOfWord() && !atStart())
        --pos;
      while (!atStartOfWord())
        --pos;
    skipBackwardOverCombining();
    break;
  case Left:
    if (!atStart())
      --pos;
    skipBackwardOverCombining();
    break;
  case NextWord:
    if (!atEnd())
      ++pos;
    while (!atStartOfWord() && !atEnd())
      ++pos;
    skipForwardOverCombining();
    break;
  case Right:
    if (!atEnd())
      ++pos;
    skipForwardOverCombining();
    break;
  case Start:
    pos = doc->firstPosition();
    break;
  case End:
    pos = doc->lastPosition();
    break;
  case Up: {
    QPointF here = doc->locate(pos);
    QPointF above = here - QPointF(0, doc->lineHeight());
    pos = doc->find(above);
    if (pos<0)
      pos = p0; //pos = doc->firstPosition();
    skipForwardOverCombining();
  } break;
  case Down: {
    QPointF here = doc->locate(pos);
    QPointF below = here + QPointF(0, doc->lineHeight());
    pos = doc->find(below);
    if (pos<0)
      pos = p0; // doc->lastPosition();
    skipForwardOverCombining();
  } break;
  case StartOfLine:
    pos = doc->lineStartFor(pos);
    break;
  case EndOfLine:
    pos = doc->lineEndFor(pos);
    break;
  case StartOfWord: {
    QRegExp re("\\W");
    int off = re.lastIndexIn(doc->text(), pos);
    if (off<doc->firstPosition())
      pos = doc->firstPosition();
    else if (off<pos)
      pos = off+1;
  } break;
  case EndOfWord: {
    QRegExp re("\\W");
    int off = re.indexIn(doc->text(), pos);
    if (off<0 || off>doc->lastPosition())
      pos = doc->lastPosition();
    else 
      pos = off;
  } break;
  case NextCell:
    qDebug() << "TextCursor: NextCell NYI";
    break;
  case PreviousCell:
    qDebug() << "TextCursor: PreviousCell NYI";
    break;
  }

  return pos!=p0;
}

void TextCursor::setPosition(int p, TextCursor::MoveMode m) {
  ASSERT(doc);
  
  if (m==KeepAnchor) {
    if (!hasSelection())
      anc = pos;
  } else {
    clearSelection();
  }
  pos = p;
  clampPosition();
}

void TextCursor::clampPosition() {
  ASSERT(doc);
  int n0 = doc->firstPosition();
  int n1 = doc->lastPosition();
  if (pos<n0)
    pos = n0;
  if (pos>n1)
    pos = n1;
  if (anc>=0) {
    if (anc<n0)
      anc = n0;
    if (anc>n1)
      anc = n1;
  }
}

TextCursor::Range TextCursor::selectedRange() const {
  if (anc<0)
    return Range(pos, pos);
  else
    return Range(pos, anc);
}

int TextCursor::selectionStart() const {
  return selectedRange().start();
}

int TextCursor::selectionEnd() const {
  return selectedRange().end();
}

bool TextCursor::hasSelection() const {
  if (anc<0)
    return false;
  if (anc!=pos)
    return true;
  anc = -1;
  return false;
}

int TextCursor::position() const {
  return pos;
}

int TextCursor::anchor() const {
  return anc;
}

bool TextCursor::operator==(TextCursor const &a) const {
  return doc==a.doc && pos==a.pos && anc==a.anc;
}

TextCursor TextCursor::findForward(QString s) const {
  if (!isValid())
    return TextCursor(doc, -1);
  QString txt = doc->text();
  int off = txt.indexOf(s, pos);
  if (off>=0)
    return TextCursor(doc, off, s.length());
  else
    return TextCursor(doc, -1);
}

TextCursor TextCursor::findBackward(QString s) const {
  if (!isValid())
    return TextCursor(doc, -1);
  QString txt = doc->text();
  int off = txt.lastIndexOf(s, pos-s.length());
  if (off>=0)
    return TextCursor(doc, off, s.length());
  else
    return TextCursor(doc, -1);
}

TextCursor TextCursor::findForward(QRegExp re) const {
  if (!isValid())
    return TextCursor(doc, -1);
  QString txt = doc->text();
  int off = re.indexIn(txt, pos);
  if (off>=0)
    return TextCursor(doc, off, off+re.cap().size());
  else
    return TextCursor(doc, -1);
}

TextCursor TextCursor::findBackward(QRegExp re) const {
  if (!isValid())
    return TextCursor(doc, -1);
  QString txt = doc->text().left(pos);
  // This makes sure that we won't capture beyond POS, 
  int off = re.lastIndexIn(txt);

  if (off>=0)
    return TextCursor(doc, off, off+re.cap().length());
  else
    return TextCursor(doc, -1);
}

void TextCursor::selectAround(int pos,
                              TextCursor::MoveOperation s,
                              TextCursor::MoveOperation e) {
  setPosition(pos);
  movePosition(s);
  movePosition(e, TextCursor::KeepAnchor);
}

void TextCursor::exchangePositionAndAnchor() {
  if (!hasSelection())
    return;
  int p = pos;
  pos = anc;
  anc = p;
}

void TextCursor::skipForwardOverCombining() {
  while (!atEnd() && Unicode::isSecondary(doc->characterAt(pos)))
    pos++;
}

void TextCursor::skipBackwardOverCombining() {
  while (!atStart() && Unicode::isSecondary(doc->characterAt(pos)))
    pos--;
}
