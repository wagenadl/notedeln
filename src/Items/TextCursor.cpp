// TextCursor.cpp

#include "TextCursor.h"
#include <QDebug>

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
}
                                                   
bool TextCursor::isValid() const {
  if (!doc)
    return false;
  int N = doc->text().size();
  return pos>=0 && pos<=N && anc<=N;
}

bool TextCursor::atStart() const {
  return pos==0;
}

bool TextCursor::atEnd() const {
  return doc ? pos==doc->text().size() : false;
}

void TextCursor::clearSelection() {
  anc = -1;
}

void TextCursor::deleteChar() {
  Q_ASSERT(doc);
  if (hasSelection()) {
    Range r = selectedRange();
    doc->remove(r.start(), r.size());
    pos = r.start();
    clearSelection();
  } else {
    doc->remove(pos, 1);
  }
}

void TextCursor::deletePreviousChar() {
  Q_ASSERT(doc);
  if (hasSelection()) {
    Range r = selectedRange();
    doc->remove(r.start(), r.size());
    pos = r.start();
    clearSelection();
  } else {
    if (!atStart()) 
      doc->remove(--pos, 1);
  }
}

TextItemDoc *TextCursor::document() {
  return doc;
}

void TextCursor::insertText(QString s) {
  Q_ASSERT(doc);
  if (hasSelection())
    deleteChar();
  doc->insert(pos, s);
  pos += s.size();
}

QString TextCursor::selectedText() const {
  Q_ASSERT(doc);
  if (!hasSelection())
    return "";
  Range r = selectedRange();
  return doc->text().mid(r.start(), r.size());
}

bool TextCursor::movePosition(TextCursor::MoveOperation op,
                              TextCursor::MoveMode m) {
  bool p0 = pos;
  
  Q_ASSERT(doc);
  
  if (m==KeepAnchor) {
    if (!hasSelection())
      anc = pos;
  } else {
    clearSelection();
  }

  switch (op) {
  case NoMove:
    break;
  case Left:
    if (!atStart())
      --pos;
    break;
  case Right:
    if (!atEnd())
      ++pos;
    break;
  case Start:
    pos=0;
    break;
  case End:
    pos = doc->text().size();
    break;
  case Up: {
    QPointF here = doc->locate(pos).topRight();
    QPointF above(here.x(), here.y() - doc->lineHeight()/2);
    pos = doc->find(above);
    if (pos<0)
      pos = 0;
  } break;
  case Down: {
    QPointF here = doc->locate(pos).bottomRight();
    QPointF below(here.x(), here.y() + doc->lineHeight()/2);
    pos = doc->find(below);
    if (pos<0)
      pos = doc->text().size();
  } break;
  case StartOfLine:
    pos = doc->lineStartFor(pos);
    break;
  case EndOfLine:
    pos = doc->lineEndFor(pos);
    break;
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
  Q_ASSERT(doc);
  
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
  Q_ASSERT(doc);
  if (pos<0)
    pos = 0;
  int N = doc->text().size();
  if (pos>N)
    pos = N;
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
  return anc>=0;
}

